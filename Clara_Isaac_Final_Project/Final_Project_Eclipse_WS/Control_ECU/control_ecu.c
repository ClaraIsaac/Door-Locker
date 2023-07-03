/******************************************************************************
 *
 * File Name: control_ecu.c
 *
 * Description: ECU responsible for control
 *
 * Author: Clara Isaac
 *
 * Date: 03/11/2022
 *
 *******************************************************************************/

#include "external_eeprom.h"
#include "dc_motor.h"
#include "buzzer.h"
#include "timer1.h"
#include "uart.h"
#include "twi.h"
#include <avr/io.h> /* To use SREG register */
#include <string.h> /* To use strcmp function */
#include <util/delay.h> /* to use delay function */

#define FAILED 0u
#define SUCCEED 1u

#define CHECK_PASS 0x10
#define INCORRECT_PASS 0x11
#define OPEN_DOOR 0x12
#define CHANGE_PASS 0x13

/*******************************************************************************
 *                       Variables Declarations                                *
 *******************************************************************************/
uint8 g_counter = 0;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
uint8 APP_comparePass(uint8 pass[]); /* get 2 passwords from the HMI ECU and check if they are matched */
void APP_savePass(void); /* save the password in EEPROM */
void APP_checkPass(void); /* check if the password entered by user is matched to the one stored in EEPROM */
void APP_openDoor(void); /* Rotate the DC motor for a specified time */
void APP_alarm(void); /* Turn On the buzzer for 1 min */
void APP_timerCounter(void); /* Callback function of Timer1 */

int main(void)
{
	/* Variables Declaration */
	uint8  a_state;
	UART_ConfigType uart_config = {EIGHT_BIT, DISABLED, ONE_STOP_BIT, 9600}; /* UART configuration */
	TWI_ConfigType twi_config = {0x01, 0x02}; /* TWI configuration */

	/* Enabling Global Interrupt Register */
	SREG |= (1<<7);

	/* DC motor initialization*/
	DcMotor_Init();
	/* Buzzer initialization*/
	Buzzer_init();
	/* UART initialization*/
	UART_init(&uart_config);
	/* TWI initialization*/
	TWI_init(&twi_config);

	APP_savePass(); /* get the password and save it in EEPROM */
	while(1)
	{
		a_state = UART_recieveByte(); /* get the state from the HMI ECU */
		switch(a_state)
		{
		case CHECK_PASS:
			APP_checkPass();
			break;
		case OPEN_DOOR:
			APP_openDoor();
			break;
		case CHANGE_PASS:
			APP_savePass();
			break;
		case INCORRECT_PASS:
			APP_alarm();
		}

	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Take 2 passwords from HMI ECU and compare them.
 */
uint8 APP_comparePass(uint8 a_pass1[])
{
	/* Variable Declaration */
	uint8 a_pass2[6];
	/* Receive the 2 passwords */
	UART_receiveString(a_pass1);
	UART_receiveString(a_pass2);

	/* Compare the 2 passwords */
	if (!(strcmp(a_pass1, a_pass2)))
		return SUCCEED;
	return FAILED;
}

/*
 * Description:
 * Keep checking if the passwords are matched or not, if yes,
 * Store the password in EEPROM.
 */
void APP_savePass(void)
{
	/* Variable Declaration */
	uint8 a_index, a_pass[6];
	/* if the 2 passwords are not matched send 'F' to Control ECU
	 * to ask for another try. */
	while(!(APP_comparePass(a_pass))){
		UART_sendByte('F'); /* Failed = not matched */
	}
	UART_sendByte('S'); /* Succeed = matched */
	/* Store the password in EEPROM */
	for(a_index = 0; a_index < 6; a_index++)
	{
		EEPROM_writeByte(0x0311+a_index, a_pass[a_index]);
		_delay_ms(10);
	}
}

/*
 * Description:
 * check if the password is matched with the one stored in EEPROM
 */
void APP_checkPass(void)
{
	/* Variables Declaration */
	uint8 a_index, a_pass[6], a_passMCU1[6];
	UART_receiveString(a_passMCU1); /* receive the password from HMI ECU */

	/* read the stored password from EEPROM */
	for(a_index = 0; a_index < 6; a_index++)
	{
		EEPROM_readByte(0x0311+a_index, &a_pass[a_index]);
		_delay_ms(10);
	}
	/* Compare the 2 passwords*/
	if (!(strcmp(a_pass, a_passMCU1)))
	{
		UART_sendByte('S'); /* Succeed = matched */
	}
	else
	{
		UART_sendByte('F'); /* Failed = not matched */
	}
}

/*
 * Description:
 * Rotate the DC motor for a specified time.
 */
void APP_openDoor(void)
{
	Timer1_ConfigType timer1_config = {0, 58593, F_CPU_1024, COMPARE_MODE}; /* timer1 configurations for 7.5 sec */
	Timer1_setCallBack(APP_timerCounter); /* set timer1 Callback function */

	DcMotor_Rotate(CLOCKWISE, 100); /* rotate the motor clockwise with max speed */
	/* Start the timer */
	Timer1_init(&timer1_config);
	while(g_counter != 2){} /* wait until it finishes counting twice (15 sec) */
	Timer1_deInit(); /* de-initialize the timer */
	g_counter = 0;

	DcMotor_Rotate(STOP, 0); /* Stop the motor */
	timer1_config.compare_value = 23437; /* change compare value for 3 sec */
	/* Start the timer */
	Timer1_init(&timer1_config);
	while(g_counter != 1){} /* wait until it finishes counting*/
	Timer1_deInit(); /* de-initialize the timer */
	g_counter = 0;

	DcMotor_Rotate(ANTI_CLOCKWISE, 100); /* rotate the motor anti-clockwise with max speed */
	timer1_config.compare_value = 58593; /* change compare value for 7.5 sec */
	/* Start the timer */
	Timer1_init(&timer1_config);
	while(g_counter != 2){} /* wait until it finishes counting twice (15 sec) */
	Timer1_deInit(); /* de-initialize the timer */
	g_counter = 0;
	DcMotor_Rotate(STOP, 0); /* Stop the motor */
}

/*
 * Description:
 * Turn On the buzzer for 1 min
 */
void APP_alarm(void)
{
	Timer1_ConfigType timer1_config = {0, 46875, F_CPU_1024, COMPARE_MODE}; /* timer1 configurations for 6 sec */
	Timer1_setCallBack(APP_timerCounter); /* set timer1 Callback function */
	Buzzer_on(); /* Turn On the buzzer */
	/* Start the timer */
	Timer1_init(&timer1_config);
	while(g_counter != 10){} /* wait until it finishes counting 10 times (1 min) */
	Timer1_deInit(); /* de-initialize the timer */
	g_counter = 0;
	Buzzer_off(); /* Turn Off the buzzer */
}

/* Callback function of Timer1 */
void APP_timerCounter(void)
{
	g_counter++;
}
