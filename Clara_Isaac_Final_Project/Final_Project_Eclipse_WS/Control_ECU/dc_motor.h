/******************************************************************************
 *
 * Module: DC motor
 *
 * File Name: dc_motor.h
 *
 * Description: Header file for the DC motor driver
 *
 * Author: Clara Isaac
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/* DC motor HW Ports and Pins Ids */
#define DC_motor_A_PORT_ID PORTB_ID
#define DC_motor_B_PORT_ID PORTB_ID

#define DC_motor_A_PIN_ID PIN0_ID
#define DC_motor_B_PIN_ID PIN1_ID

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum{
	STOP, CLOCKWISE, ANTI_CLOCKWISE
} DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Initialize the DC motor:
 * 1. Setup the DC motor pins directions by use the GPIO driver.
 * 2. Initialize Values on pins.
 */
void DcMotor_Init(void);

/*
 * Description:
 * Rotate the motor with a specified speed and direction
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
