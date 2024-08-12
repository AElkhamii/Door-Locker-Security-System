 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the AVR Buzzer driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/*******************************************************************************
 *                    	     	Include Header	                               *
 *******************************************************************************/
#include "std_types.h"

/******************************************************************************
 *									 Definitions							  *
 ******************************************************************************/
#define BUZZER_PORT_ID		PORTC_ID
#define BUZZER_PIN_ID		PIN7_ID

/*******************************************************************************
 *                         	Function Prototypes                                *
 *******************************************************************************/
/*
 * Description:
 * Activate the Buzzer pin as output bin.
 */
void BUZZER_init(void);

/*
 * Description:
 * Make the buzzer produce sound.
 */
void BUZZER_on(void);

/*
 * Description:
 * Mute the buzzer.
 */
void BUZZER_off(void);

#endif /* BUZZER_H_ */
