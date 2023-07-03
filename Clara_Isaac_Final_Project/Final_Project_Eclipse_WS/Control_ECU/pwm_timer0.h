/******************************************************************************
 *
 * Module: PWM Timer0
 *
 * File Name: pwm_timer0.h
 *
 * Description: Header file for the PWM Timer0 driver
 *
 * Author: Clara Isaac
 *
 *******************************************************************************/

#ifndef PWM_TIMER0_H_
#define PWM_TIMER0_H_

#include "std_types.h"

/*
 * Description:
 * Start the Timer at PWM mode and generate the wave at PB3
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* PWM_TIMER0_H_ */
