/******************************************************************************
 *
 * File Name: hmi_ecu.c
 *
 * Description: ECU responsible for human interface
 *
 * Author: Clara Isaac
 *
 * Date: 03/11/2022
 *
 *******************************************************************************/

#include "std_types.h"
#include "common_macros.h"
#include "timer1.h"
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include <avr/io.h> /* To use SREG register */
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

void APP_setPass(void); /* set the password */
void APP_getPassFromUser(void); /* get the password from user */
uint8 APP_checkPass(void); /* take the password from user and check if it is matched with the fixed one */
void APP_openDoor(void); /* printing on the LCD the state of the door */
void APP_alarm(void); /* printing on the LCD while the buzzer is on */
void APP_timerCounter(void); /* Timer1 Callback function */

int main(void)
{
	/* Variables Declaration */
	uint8 a_checkPass, a_choice;
	UART_ConfigType uart_config = {EIGHT_BIT, DISABLED, ONE_STOP_BIT, 9600}; /* UART configuration */

	/* Enabling Global Interrupt Register */
	SREG |= (1<<7);

	/* UART initialization */
	UART_init(&uart_config);
	/* LCD initialization */
	LCD_init();

	a_checkPass = 'F';
	/* Set the password before anything */
	while(a_checkPass != 'S') /* loop until the user enters the SAME password twice */
	{
		APP_setPass();
		a_checkPass = UART_recieveByte(); /* receive from the second ECU if they are matched 'S' or not 'F' */
	}

	while(1)
	{
		/* Print the list of options on LCD */
		LCD_clearScreen();
		LCD_displayString("+ : Open Door");
		LCD_moveCursor(1,0);
		LCD_displayString("- : Change Pass");

		/* Take the choice from user */
		do
			a_choice = KEYPAD_getPressedKey();
		while((a_choice != '+') && (a_choice != '-')); /* other than the two options do nothing */

		if (a_choice == '+')
		{
			/* take the password and check on it max 3 times */
			if(APP_checkPass())
				APP_openDoor();
			else
			{
				APP_alarm();
			}
		}
		else if(a_choice == '-')
		{
			/* take the password and check on it max 3 times */
			if(APP_checkPass())
			{
				/* Reset the password */
				a_checkPass = 'F';
				UART_sendByte(CHANGE_PASS);
				while(a_checkPass != 'S') /* loop until the user enters the SAME password twice */
				{
					APP_setPass();
					a_checkPass = UART_recieveByte(); /* receive from the second ECU if they are matched 'S' or not 'F' */
				}
			}
			else
			{
				APP_alarm();
			}
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Take the password twice for confirmation and send them to the control ECU.
 */
void APP_setPass(void)
{
	/* Variables declarations */
	uint8 a_index = 0, a_pass1[7] = "96321", a_pass2[7] = "96321#";

	/* Get the password from user */
	LCD_clearScreen();
	LCD_displayString("Plz enter pass: ");
	LCD_moveCursor(1,0);
	while(1)
	{
		if (a_index == 5)
		{
			a_pass1[a_index] = KEYPAD_getPressedKey();
			if (a_pass1[a_index] == 13) /* ENTER (the user have finish entering the password) */
			{
				a_index = 0;
				break;
			}

		}
		else
		{
			/* Password consists of numbers only */
			a_pass1[a_index] = KEYPAD_getPressedKey();
			if((a_pass1[a_index] >= '0') &&  (a_pass1[a_index] <= '9'))
			{
				LCD_displayCharacter('*');
				a_index++;
			}
		}
		_delay_ms(500); /* time of press */
	}

	/* Get the password from user a second time*/
	LCD_clearScreen();
	LCD_displayString("Plz re-enter the ");
	LCD_moveCursor(1,0);
	LCD_displayString("same pass: ");
	LCD_moveCursor(1, 11);
	while(1)
	{
		if (a_index == 5)
		{
			a_pass2[a_index] = KEYPAD_getPressedKey();
			if (a_pass2[a_index] == 13) /* ENTER (the user have finish entering the password) */
			{
				break;
			}
		}
		else
		{
			/* Password consists of numbers only */
			a_pass2[a_index] = KEYPAD_getPressedKey();
			if((a_pass2[a_index] >= '0') &&  (a_pass2[a_index] <= '9'))
			{
				LCD_displayCharacter('*');
				a_index++;
			}
		}
		_delay_ms(500); /* time of press */
	}
	a_pass1[5] = '#';
	a_pass2[5] = '#';
	a_pass1[6] = '\0';
	a_pass2[6] = '\0';
	/* send the 2 inputs to Control ECU */
	UART_sendString(a_pass1);
	UART_sendString(a_pass2);
}

/*
 * Description:
 * Get password from user and send it to control ECU.
 */
void APP_getPassFromUser(void)
{
	uint8 a_index = 0, a_pass[7] = "96321#";
	LCD_clearScreen();
	LCD_displayString("Plz enter pass: ");
	LCD_moveCursor(1,0);
	while(1)
	{
		if (a_index == 5)
		{
			a_pass[a_index] = KEYPAD_getPressedKey(); /* ENTER (the user have finish entering the password) */
			if (a_pass[a_index] == 13)
			{
				break;
			}
		}
		else
		{
			/* Password consists of numbers only */
			a_pass[a_index] = KEYPAD_getPressedKey();
			if((a_pass[a_index] >= '0') &&  (a_pass[a_index] <= '9'))
			{
				LCD_displayCharacter('*');
				a_index++;
			}
		}
		_delay_ms(500); /* time of press */
	}
	a_pass[5] = '#';
	a_pass[6] = '\0';
	/* send the input to Control ECU */
	UART_sendString(a_pass);
}

/*
 * Description:
 * Display on the LCD "Door is unlocking for 18 seconds (15 CW + 3 Stop), then
 * Display on the LCD "Door is unlocking for 15 seconds (15 ACW).
 */
void APP_openDoor(void)
{
	Timer1_ConfigType timer1_config = {0, 0, F_CPU_1024, COMPARE_MODE}; /* Timer configurations */
	/* Set the callback function */
	Timer1_setCallBack(APP_timerCounter);
	UART_sendByte(OPEN_DOOR); /* Announce the control ECU that we are in the state of opening the door */
	/* Prepare the LCD */
	LCD_clearScreen();
	LCD_displayString("Door is ");
	LCD_moveCursor(1,0);
	LCD_displayString("unlocking");
	/* set the compare value */
	timer1_config.compare_value = 17578; /* for 18 sec */
	/* Start the timer*/
	Timer1_init(&timer1_config);
	while(g_counter != 1){} /* wait until it finishes counting*/
	Timer1_deInit(); /* de-initialize the timer */
	g_counter = 0;

	/* Prepare the LCD */
	LCD_moveCursor(1,0);
	LCD_displayString("locking   ");
	/* set the compare value */
	timer1_config.compare_value = 14648; /* for 15 sec */
	/* Start the timer*/
	Timer1_init(&timer1_config);
	while(g_counter != 1){} /* wait until it finishes counting*/
	Timer1_deInit(); /* de-initialize the timer */
	g_counter = 0;
}

/*
 * Description:
 * take the password from user and check if it is matched with the fixed one for maximum 3 times.
 */
uint8 APP_checkPass(void)
{
	/* Variable declaration */
	uint8 a_counter = 0, a_checkPass = 'F';
	/* loop until the password matches with the fixed one or the counter reaches 3.*/
	while(a_checkPass != 'S')
	{
		if (a_counter == 3)
			break;
		UART_sendByte(CHECK_PASS); /* Tell the control ECU to be ready for comparing the passwords */
		APP_getPassFromUser(); /* user enters the password*/
		a_checkPass = UART_recieveByte();
		a_counter++;
	}
	if (a_checkPass == 'F') /* the loop breaks due to break (counter = 3)*/
		return FAILED;
	return SUCCEED;
}

/*
 * Description:
 * Display on LCD "INCORRECT PASS" while the buzzer is on for 1 min.
 */
void APP_alarm(void)
{
	Timer1_ConfigType timer1_config = {0, 58593, F_CPU_1024, COMPARE_MODE}; /* Timer configurations */
	Timer1_setCallBack(APP_timerCounter); /* Set the callback function */
	/* Prepare the LCD */
	LCD_clearScreen();
	LCD_displayString("INCORRECT PASS");
	UART_sendByte(INCORRECT_PASS); /* Announce the control ECU that we are in the state of incorrect password */
	/* Start the timer*/
	Timer1_init(&timer1_config);
	while(g_counter != 1){} /* wait until it finishes counting*/
	g_counter = 0;
	Timer1_deInit(); /* de-initialize the timer */
}

/* Callback function of Timer1 */
void APP_timerCounter(void)
{
	g_counter++;
}
