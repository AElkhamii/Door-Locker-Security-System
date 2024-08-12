/****************************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.c
 *
 * Discretion: Source file for the AVR LCD driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/

/*******************************************************************************
 *                      		Include Header	                               *
 *******************************************************************************/
#include "lcd.h"
#include <avr/io.h>
#include "gpio.h"
#include <util/delay.h>

/*******************************************************************************
 *                      	Function Definitions                               *
 *******************************************************************************/
/*
 * Description:
 * This function activate LCD pins and start the LCD by identify LCD mode and clear the screen.
 */
void LCD_init(void)
{
	/* Make control pins output */
	GPIO_setupPinDirection(LCD_RS_PORT_ID, LCD_RS_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_RW_PORT_ID, LCD_RW_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_E_PORT_ID, LCD_E_PIN_ID, PIN_OUTPUT);
#if(LCD_DATA_BITS_MODE == 8)
	/* Make Data port output */
	GPIO_setupPortDirection(LCD_DATA_PORT_ID, PORT_OUTPUT);

	LCD_sendCommand(LCD_TWO_LINES_EIGHT_BITS_MODE); /* Two lines 8-bit mode */
#elif(LCD_DATA_BITS_MODE == 4)

	/* Make Data pins output */
	GPIO_setupPinDirection(LCD_DATA_PORT_ID, LCD_FIRST_DATA_PIN_ID + 0, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID, LCD_FIRST_DATA_PIN_ID + 1, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID, LCD_FIRST_DATA_PIN_ID + 2, PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID, LCD_FIRST_DATA_PIN_ID + 3, PIN_OUTPUT);

	LCD_sendCommand(LCD_RETURN_HOME	);
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE); /* Two lines 4-bit mode */
#endif
	/* Send initial commands */
	LCD_sendCommand(LCD_CURSOR_OFF); /* Cursor off*/
	LCD_sendCommand(LCD_CLEAR_SCREEN ); /* Clear Screen */
}

/*
 * Description:
 * This function send commands to the LCD.
 */
void LCD_sendCommand(uint8 command)
{
	uint8 lcd_port_value = 0;
	/* RS = 0  (to send command) and R/W = 0 (to write value) */
	GPIO_writePin(LCD_RS_PORT_ID, LCD_RS_PIN_ID, LOGIC_LOW);
	GPIO_writePin(LCD_RW_PORT_ID, LCD_RW_PIN_ID, LOGIC_LOW);

	_delay_ms(1);

	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_HIGH); /* Enable(E) = 1 */

	_delay_ms(1);

#if(LCD_DATA_BITS_MODE == 8)
	/* Send command */
	GPIO_writePort(LCD_DATA_PORT_ID, command);

	_delay_ms(1);

	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_LOW); /* Enable(E) = 0 */

	_delay_ms(1);
#elif(LCD_DATA_BITS_MODE == 4)
	/* out the last 4 bits of the required command to the data bus D4 --> D7 */
	lcd_port_value = GPIO_readPort(LCD_DATA_PORT_ID);
#ifdef LCD_LAST_PORT_PINS
	lcd_port_value = (lcd_port_value & 0x0F) | (command & 0xF0);
#else
	lcd_port_value = (lcd_port_value & 0xF0) | ((command & 0xF0) >> 4);
#endif
	GPIO_writePort(LCD_DATA_PORT_ID,lcd_port_value);

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH); /* Enable LCD E=1 */
	_delay_ms(1);

	/* out the first 4 bits of the required command to the data bus D4 --> D7 */
	lcd_port_value = GPIO_readPort(LCD_DATA_PORT_ID);
#ifdef LCD_LAST_PORT_PINS
	lcd_port_value = (lcd_port_value & 0x0F) | ((command & 0x0F) << 4);
#else
	lcd_port_value = (lcd_port_value & 0xF0) | (command & 0x0F);
#endif
	GPIO_writePort(LCD_DATA_PORT_ID,lcd_port_value);

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1);

#endif

}


/*
 * Description:
 * This function send character to the LCD.
 */
void LCD_displayCharacter(uint8 character)
{
	uint8 lcd_port_value = 0;
	/* RS = 1 (to send character) and R/W = 0 (to write value) */
	/* RS = 0  (to send command) and R/W = 0 (to write value) */
	GPIO_writePin(LCD_RS_PORT_ID, LCD_RS_PIN_ID, LOGIC_HIGH);
	GPIO_writePin(LCD_RW_PORT_ID, LCD_RW_PIN_ID, LOGIC_LOW);

	_delay_ms(1);

	/* Enable(E) = 1 */
	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_HIGH);

	_delay_ms(1);

#if(LCD_DATA_BITS_MODE == 8)
	/* Send character */
	GPIO_writePort(LCD_DATA_PORT_ID, character);

	_delay_ms(1);

	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_LOW); /* Enable(E) = 0 */

	_delay_ms(1);
#elif(LCD_DATA_BITS_MODE == 4)
	/* out the last 4 bits of the required data to the data bus D4 --> D7 */
	lcd_port_value = GPIO_readPort(LCD_DATA_PORT_ID);
#ifdef LCD_LAST_PORT_PINS
	lcd_port_value = (lcd_port_value & 0x0F) | (character & 0xF0);
#else
	lcd_port_value = (lcd_port_value & 0xF0) | ((character & 0xF0) >> 4);
#endif
	GPIO_writePort(LCD_DATA_PORT_ID,lcd_port_value);

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH); /* Enable LCD E=1 */
	_delay_ms(1);

	/* out the first 4 bits of the required data to the data bus D4 --> D7 */
	lcd_port_value = GPIO_readPort(LCD_DATA_PORT_ID);
#ifdef LCD_LAST_PORT_PINS
	lcd_port_value = (lcd_port_value & 0x0F) | ((character & 0x0F) << 4);
#else
	lcd_port_value = (lcd_port_value & 0xF0) | (character & 0x0F);
#endif
	GPIO_writePort(LCD_DATA_PORT_ID,lcd_port_value);

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW); /* Disable LCD E=0 */
	_delay_ms(1);
#endif
}

/*
 * Description:
 * This function send a string to the LCD by loop on that string.
 */
void LCD_displayString(const uint8* string)
{
	uint8 i; /*counter*/
	/* This will loop on the string to display characters beside each other on LCD */
	for(i=0;(*string) != '\0';i++)
	{
		/* Display character on the LCD */
		LCD_displayCharacter (*string);
		/* Jump to the next character */
		string++;
	}
}

/*
 * Description:
 * This function move the cursor in memory to choose position on LCD to start with.
 */
void LCD_moveCursor(uint8 row, uint8 col)
{
	uint8 lcd_memory_address;

	/* Calculate the required address in the LCD DDRAM */
	switch(row)
	{
		case 0:
			lcd_memory_address=col;
				break;
		case 1:
			lcd_memory_address=col+0x40;
				break;
		case 2:
			lcd_memory_address=col+0x10;
				break;
		case 3:
			lcd_memory_address=col+0x50;
				break;
	}
	/* Move the LCD cursor to this specific address */
	LCD_sendCommand(lcd_memory_address | LCD_SET_CURSOR_LOCATION); /*1000 0000 | memory address*/
}

/*
 * Description:
 * This function can select the position of the cursor and and write a string directly.
 */
void LCD_displayStringRowColumn(uint8 row, uint8 col, const uint8* string)
{
	LCD_moveCursor(row, col);  /* Go to the required LCD position */
	LCD_displayString(string); /* Display the string */

}


/*
 * Description:
 * This function convert integer number into ASSCI to present the value on the LCD.
 */
void LCD_intgerToString(int intiger)
{
	   uint8 buff[16]; /* String to hold the ASCII result */
	   itoa(intiger,buff,10); /* Use itoa C function to convert the data to its corresponding ASCII value, 10 for decimal */
	   LCD_displayString(buff); /* Display the string */
}

/*
 * Description:
 * This function clear the LCD screen.
 */
void LCD_clearScreen(void)
{
	LCD_sendCommand(LCD_CLEAR_SCREEN ); /* Clear Screen */
}

