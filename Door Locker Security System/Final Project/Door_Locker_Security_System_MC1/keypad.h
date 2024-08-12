/****************************************************************************************
 *
 * Module: keypad
 *
 * File Name: keypad.h
 *
 * Discretion: Header file for the AVR Keypad driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/


#ifndef KEYPAD_H_
#define KEYPAD_H_

/******************************************************************************
 *									 Include Header							  *
 ******************************************************************************/
#include "std_types.h"


/******************************************************************************
 *									 Definitions							  *
 ******************************************************************************/
/* Keypad configurations for number of rows and columns */
#define NUMBER_OF_COLUMNS					4
#define NUMBER_OF_ROW 						4

/* Keypad Port Configurations */
#define KEYPAD_PORT_ID 						PORTA_ID

#define KEYPAD_FIRST_ROW_PIN 				PIN0_ID
#define KEYPAD_FIRST_COLUMN_BIN 			PIN4_ID

/* Keypad button logic configurations */
#define BUTTON_IS_PRESSED 					LOGIC_LOW		/* lOGIC_LOW for bull-up & LOGIC_HIGH for bull-down */
#define BUTTON_IS_RELEASED					LOGIC_HIGH		/* lOGIC_HIGH for bull-up & LOGIC_LOW for bull-down */


/******************************************************************************
 *								 Function Prototypes						  *
 ******************************************************************************/

/*
 * Description:
 * This function is used to get the value of the button that pressed by the user.
 * this function loop on columns and rows to get the value of the button that the user pressed.
 */
uint8 KEYPAD_getPressedKey(void);

#endif
