/******************************************************************************
 *
 * Module: PWM Timer0
 *
 * File Name: pwm_timer0.c
 *
 * Description: Source file for the PWM Timer0 driver
 *
 * Author: Clara Isaac
 *
 *******************************************************************************/

#include <avr/io.h> /* to use timer0 registers */
#include "pwm_timer0.h"
#include "gpio.h"

/*
 * Description:
 * Start the Timer at PWM mode and generate the wave at PB3
 */
void PWM_Timer0_Start(uint8 duty_cycle)
{
	if(duty_cycle == 0)
		TCCR0 = 0; /* stop the timer */
	else
	{
		GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT); /* setup PB3 as Output Pin*/

		TCNT0 = 0; /* initialize to 0*/
		OCR0  = duty_cycle ; /* compare value*/
		/*
		 * FOC0 = 0 (PWM mode)
		 * WGM01 = 1 & WGM00 = 1 (Fast PWM mode)
		 * COM01 = 1 & COM00 = 0 (non inverting mode)
		 * CS02 = 0 & CS01 = 1 & CS02 = 0 (prescaler = 8)
		 */
		TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
	}
}
