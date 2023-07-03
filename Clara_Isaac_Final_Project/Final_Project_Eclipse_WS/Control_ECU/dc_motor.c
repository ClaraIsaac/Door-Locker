/******************************************************************************
 *
 * Module: DC motor
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for the DC motor driver
 *
 * Author: Clara Isaac
 *
 *******************************************************************************/

#include "dc_motor.h"
#include "pwm_timer0.h" /* To use the PWM Function */
#include "gpio.h"
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Initialize the DC motor:
 * 1. Setup the DC motor pins directions by use the GPIO driver.
 * 2. Initialize Values on pins.
 */
void DcMotor_Init(void)
{
	/* Configure the direction for A, B pins as output pins */
	GPIO_setupPinDirection(DC_motor_A_PORT_ID, DC_motor_A_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_motor_B_PORT_ID, DC_motor_B_PIN_ID, PIN_OUTPUT);

	/* Stop the motor as initialization */
	GPIO_writePin(DC_motor_A_PORT_ID, DC_motor_A_PIN_ID, LOGIC_LOW);
	GPIO_writePin(DC_motor_B_PORT_ID, DC_motor_B_PIN_ID, LOGIC_LOW);
}

/*
 * Description:
 * Rotate the motor with a specified speed and direction
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	uint8 duty_cycle = (uint8)(((uint16)speed * 255) / 100); /* Take the percentage speed and multiply by (2^8 - 1) */
	PWM_Timer0_Start(duty_cycle); /* Send the duty cycle to PWM function to generate the PWM signal */

	switch(state)
	{
	/* Stop the motor */
	case STOP:
		/* A = 0, B = 0 */
		GPIO_writePin(DC_motor_A_PORT_ID, DC_motor_A_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_motor_B_PORT_ID, DC_motor_B_PIN_ID, LOGIC_LOW);
		break;
		/* Clockwise direction */
	case CLOCKWISE:
		/* A = 1, B = 0 */
		GPIO_writePin(DC_motor_A_PORT_ID, DC_motor_A_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(DC_motor_B_PORT_ID, DC_motor_B_PIN_ID, LOGIC_LOW);
		break;
		/* Anti-clockwise direction */
	case ANTI_CLOCKWISE:
		/* A = 0, B = 1 */
		GPIO_writePin(DC_motor_A_PORT_ID, DC_motor_A_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_motor_B_PORT_ID, DC_motor_B_PIN_ID, LOGIC_HIGH);
		break;
	}
}
