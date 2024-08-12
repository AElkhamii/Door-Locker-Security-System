 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the AVR Buzzer driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/

/*******************************************************************************
 *                    	     	Include Header	                               *
 *******************************************************************************/
#include "buzzer.h"
#include "gpio.h"

/*******************************************************************************
 *                         	Function Deceleration                              *
 *******************************************************************************/

/*
 * Description:
 * Activate the Buzzer pin as output bin.
 */
void BUZZER_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT_ID, BUZZER_PIN_ID, PIN_OUTPUT); /* Activate buzzer pin */
}

/*
 * Description:
 * Make the buzzer produce sound.
 */
void BUZZER_on(void)
{
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_HIGH); /* buzzer is on when the output is logically high */
}

/*
 * Description:
 * Mute the buzzer.
 */
void BUZZER_off(void)
{
	GPIO_writePin(BUZZER_PORT_ID, BUZZER_PIN_ID, LOGIC_LOW); /* buzzer is off when the output is logically low */
}
