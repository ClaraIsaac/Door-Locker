/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the Buzzer driver
 *
 * Author: Clara Isaac
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/* Buzzer HW Port and Pin ID*/
#define BUZZER_PORT PORTC_ID
#define BUZZER_PIN PIN3_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description:
 * Set the Buzzer Pin as output Pin using GPIO.
 */
void Buzzer_init(void);

/*
 * Description:
 * Turn On the Buzzer.
 */
void Buzzer_on(void);

/*
 * Description:
 * Turn Off the Buzzer.
 */
void Buzzer_off(void);

#endif /* BUZZER_H_ */
