/****************************************************************************************
 *
 * Module: PWM
 *
 * File Name: pwm.c
 *
 * Discretion: Source file for the AVR PWM driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/

/*******************************************************************************
 *                    	     	Include Header	                               *
 *******************************************************************************/
#include "pwm.h"
#include <avr/io.h>
#include "gpio.h"

/*******************************************************************************
 *                    	    Functions Declaration                              *
 *******************************************************************************/
/*
 * Description:
 * Generate PWM with wanted speed percentage.
 */
void PWM_Timer2_init(uint8 duty_cycle)
{
	TCNT2 = 0; /* Initial value */

	OCR2 = 255 * (duty_cycle * 0.01); /* Set Compare value */

	DDRB = DDRB | (1<<EN_PIN_ID);
	/*
	 * FOC0 = 0 (because PWM is is used)
	 * COM00 = 0, COM01 = 1 (Non-inveting mode)
	 * CS00 =0 CS01 = 1 CS02 = 0 (F_CPU/8)
	 */
	TCCR2 = (1<<WGM21) | (1<<WGM20) | (1<<COM21) | (1<<CS21);
}
