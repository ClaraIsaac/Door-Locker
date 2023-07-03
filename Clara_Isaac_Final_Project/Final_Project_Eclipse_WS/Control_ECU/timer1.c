/******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.c
 *
 * Description: Source file for the Timer1 driver
 *
 * Author: Clara Isaac
 *
 *******************************************************************************/

#include "timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the timer is fired */
		(*g_callBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the timer is fired */
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Initialize timer registers:
 * TCNT1, OCR1A, TIMSK, TCCR1A, TCCR1B
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	/* Set timer1 initial count to zero */
	TCNT1 = Config_Ptr->initial_value;
	/* Set the compare Value*/
	OCR1A = Config_Ptr->compare_value;
	/* Configure timer control register TIMSK
	 * Normal Mode: (1<<2) = (1<<TOIE1)
	 * Compare Mode: (1<<4) = (1<<OCIE1A)
	 */
	TIMSK |= (1<<Config_Ptr->mode);
	/* Configure timer control register TCCR1A
	 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
	 * 2. FOC1A=1 FOC1B=1
	 * 3. Mode: WGM10=0 WGM11=0 (Bit 0,1) always zero as Normal Mode = 0 (0b0000)and Compare Mode = 4 (0b1000)
	 */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	/* Configure timer control register TCCR1B
	 * 1. Mode: WGM12=1 WGM13=0 (Bit 3,4)
	 * 2. Prescaler: CS10 CS11 CS12 (Bit 0,1,2)
	 */
	TCCR1B = (TCCR1B & 0xE7) | (((Config_Ptr->mode & 0x0C) >> 2) << 3);
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);

}

/*
 * Description:
 * Disable timer1:
 * Clear TIMSK, TCCR1A, TCCR1B registers.
 */
void Timer1_deInit(void)
{
	TIMSK &= 0xC3;
	TCCR1A = 0;
	TCCR1B = 0;
}

/*
 * Description:
 * Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void))
{
	g_callBackPtr = a_ptr;
}


