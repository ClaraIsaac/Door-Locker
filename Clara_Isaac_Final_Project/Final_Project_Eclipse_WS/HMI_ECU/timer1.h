/******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.h
 *
 * Description: Header file for the Timer1 driver
 *
 * Author: Clara Isaac
 *
 *******************************************************************************/

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum{
	NO_CLOCK, F_CPU_1, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024
}Timer1_Prescaler;

typedef enum{
	NORMAL_MODE = 2, COMPARE_MODE = 4
}Timer1_Mode;

typedef struct {
 uint16 initial_value;
 uint16 compare_value; // it will be used in compare mode only.
 Timer1_Prescaler prescaler;
 Timer1_Mode mode;
} Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description:
 * Initialize timer registers:
 * TCNT1, OCR1A, TIMSK, TCCR1A, TCCR1B
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*
 * Description:
 * Disable timer1:
 * Clear TIMSK, TCCR1A, TCCR1B registers.
 */
void Timer1_deInit(void);

/*
 * Description:
 * Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void));

#endif /* TIMER1_H_ */
