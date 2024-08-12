/****************************************************************************************
 *
 * Module: DC-MOTOR
 *
 * File Name: dc_motor.c
 *
 * Discretion: Source file for the AVR DC-motor driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/

/*******************************************************************************
 *                    	     	Include Header	                               *
 *******************************************************************************/
#include "dc_motor.h"
#include "pwm.h"
#include <avr/io.h>
#include "gpio.h"


/*******************************************************************************
 *                    	    Functions Declaration                              *
 *******************************************************************************/
/*
 * Description:
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DCMotor_init(void)
{
	/* Select the pins that the motor are connected with */
	GPIO_setupPinDirection(DC_PORT_ID, DC_PINA_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_PORT_ID, DC_PINB_ID, PIN_OUTPUT);

	/* Initial condition to stop the motor */
	GPIO_writePin(DC_PORT_ID, DC_PINA_ID, LOGIC_LOW);
	GPIO_writePin(DC_PORT_ID, DC_PINB_ID, LOGIC_LOW);
}

/*
 * Description:
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
 *  Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DCMotor_rotate(DcMotor_State state,uint8 speed)
{

	PWM_Timer2_init(speed); /* Adjust PWM according to the speed percentage the user want */

	/* State of the motor */
	if(state == STOP)
	{
		GPIO_writePin(DC_PORT_ID, DC_PINA_ID, LOGIC_LOW);
		GPIO_writePin(DC_PORT_ID, DC_PINB_ID, LOGIC_LOW);
	}
	else if(state == CW)
	{
		GPIO_writePin(DC_PORT_ID, DC_PINA_ID, LOGIC_HIGH);
		GPIO_writePin(DC_PORT_ID, DC_PINB_ID, LOGIC_LOW);
	}
	else if(state == CCW)
	{
		GPIO_writePin(DC_PORT_ID, DC_PINA_ID, LOGIC_LOW);
		GPIO_writePin(DC_PORT_ID, DC_PINB_ID, LOGIC_HIGH);
	}
}



