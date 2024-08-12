/****************************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.h
 *
 * Discretion: Header file for the AVR LCD driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/

#ifndef LCD_H_
#define LCD_H_

/*******************************************************************************
 *                      		Include Header	                               *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                      		Definitions 	                               *
 *******************************************************************************/
/* Mode sellect */
#define LCD_DATA_BITS_MODE 8

#if((LCD_DATA_BITS_MODE != 4) && (LCD_DATA_BITS_MODE != 8))

#error "Number of Data bits should be equal to 4 or 8"

#endif

#if (LCD_DATA_BITS_MODE == 4)

/* if LCD_LAST_PORT_PINS is defined in the code, the LCD driver will use the last 4 pins in the gpio port for for data.
 * To use the first four pins in the gpio port for data just remove LCD_LAST_PORT_PINS */
#define LCD_LAST_PORT_PINS

#ifdef LCD_LAST_PORT_PINS
#define LCD_FIRST_DATA_PIN_ID         PIN4_ID
#else
#define LCD_FIRST_DATA_PIN_ID         PIN0_ID
#endif

#endif

/* LCD ports and pins configurations */
#define LCD_RS_PORT_ID					PORTD_ID
#define LCD_RS_PIN_ID  					PIN4_ID

#define LCD_RW_PORT_ID					PORTD_ID
#define LCD_RW_PIN_ID 					PIN5_ID

#define LCD_E_PORT_ID					PORTD_ID
#define LCD_E_PIN_ID					PIN6_ID

/* 8-bit mode port configurations */
#define LCD_DATA_PORT_ID				PORTC_ID


/* LCD Common commands */
#define LCD_CLEAR_SCREEN 				0x01
#define LCD_RETURN_HOME					0x02
#define LCD_TWO_LINES_EIGHT_BITS_MODE   0x38
#define LCD_TWO_LINES_FOUR_BITS_MODE    0x28
#define LCD_CURSOR_OFF					0x0C
#define LCD_CURSOR_ON					0x0E
#define LCD_SET_CURSOR_LOCATION         0x80


/*******************************************************************************
 *                 	 	    Functions Prototypes                               *
 *******************************************************************************/
/*
 * Description:
 * This function activate LCD pins and start the LCD by identify LCD mode and clear the screen.
 */
void LCD_init(void);

/*
 * Description:
 * This function send commands to the LCD.
 */
void LCD_sendCommand(uint8 command);

/*
 * Description:
 * This function send character to the LCD.
 */
void LCD_displayCharacter(uint8 character);

/*
 * Description:
 * This function send a string to the LCD by loop on that string.
 */
void LCD_displayString(const uint8* string);

/*
 * Description:
 * This function move the cursor in memory to choose position on LCD to start with.
 */
void LCD_moveCursor(uint8 row, uint8 col);

/*
 * Description:
 * This function can select the position of the cursor and and write a string directly.
 */

void LCD_displayStringRowColumn(uint8 row, uint8 col, const uint8* string);
/*
 * Description:
 * This function convert integer number into ASSCI to present the value on the LCD.
 */
void LCD_intgerToString(int intiger);

/*
 * Description:
 * This function clear the LCD screen.
 */
void LCD_clearScreen(void);



#endif /* LCD_H_ */
