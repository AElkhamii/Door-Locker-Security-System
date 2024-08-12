/****************************************************************************************
 *
 * Module: DC-MOTOR
 *
 * File Name: dc_motor.h
 *
 * Discretion: Header file for the AVR DC-motor driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

/*******************************************************************************
 *                      		Include Header	                               *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define DC_PORT_ID 			PORTB_ID
#define DC_PINA_ID			PIN0_ID
#define DC_PINB_ID			PIN1_ID
#define EN_PIN_ID			PIN3_ID
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	STOP, CW, CCW
}DcMotor_State;

/*******************************************************************************
 *                      	Function Definitions                               *
 *******************************************************************************/
/*
 * Description:
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DCMotor_init(void);

/*
 * Description:
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
 *  Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DCMotor_rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
