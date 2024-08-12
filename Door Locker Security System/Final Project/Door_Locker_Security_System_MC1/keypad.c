 /******************************************************************************
 *
 * Module: Keypad
 *
 * File Name: keypad.c
 *
 * Description: Source file for the AVR keypad driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/


#include "keypad.h"
#include "gpio.h"
#include <avr\io.h>

#if (NUMBER_OF_COLUMNS == 3)
/*
 * Function responsible for mapping the switch number in the keypad to
 * its corresponding functional number in the proteus for 4x3 keypad
 */
static uint8 KEYPAD_4x3_adjustKeyNumber(uint8 button_number);
#elif (NUMBER_OF_COLUMNS == 4)
/*
 * Function responsible for mapping the switch number in the keypad to
 * its corresponding functional number in the proteus for 4x4 keypad
 */
static uint8 KEYPAD_4x4_adjustKeyNumber(uint8 button_number);
#endif

/*
 * Description:
 * This function is used to get the value of the button that pressed by the user.
 * this function loop on columns and rows to get the value of the button that the user pressed.
 */
uint8 KEYPAD_getPressedKey(void)
{
	uint8 col, row;
	uint8 keypad_port_value = 0;

	while(1)
	{
		/* This will loop on the keypad columns */
		for(col=0;col<NUMBER_OF_COLUMNS;col++)
		{
			/* Make the whole port output and change direction of a certain pin each loop to output */
			GPIO_setupPortDirection(KEYPAD_PORT_ID , PORT_INPUT);
			GPIO_setupPinDirection(KEYPAD_PORT_ID , col + PIN4_ID, PIN_OUTPUT);

			/*
			 * This value will make that certain pin value equal to 0 while the other values of the whole port equal to 1
			 * The AVR open internal pull up when we put ones in pins 0 ~ 3
			 */
#if(BUTTON_IS_PRESSED == LOGIC_LOW)
			keypad_port_value = ~(1<<(col + PIN4_ID));
#elif(BUTTON_IS_PRESSED == LOGIC_HIGH)
			keypad_port_value = (1<<(col + PIN4_ID));
#endif
			/* Write the value in the port */
			GPIO_writePort(KEYPAD_PORT_ID , keypad_port_value);

			/* This will loop on keypad rows */
			for(row=0;row<NUMBER_OF_ROW;row++)
			{
				/* Check for each row if the button is pressed */
				if(GPIO_readPin(KEYPAD_PORT_ID ,row) == BUTTON_IS_PRESSED)
				{
					/* Return button number */
#if(NUMBER_OF_COLUMNS == 3)
					return KEYPAD_4x3_adjustKeyNumber((row*NUMBER_OF_COLUMNS)+col+1);
#elif (NUMBER_OF_COLUMNS == 4)
					return KEYPAD_4x4_adjustKeyNumber((row*NUMBER_OF_COLUMNS)+col+1);
#endif

				}
			}
		}
	}
}

#if(NUMBER_OF_COLUMNS == 3)
/*
 * This function used to adjust the buttons in 4x3 keypad to its right presentation.
 */
static uint8 KEYPAD_4x3_adjustKeyNumber(uint8 button_number)
{
	uint8 keypad_button = 0;
	switch(button_number)
	{
	case 10:
		keypad_button = '*'; // ASCII OF *
		break;
	case 11:
		keypad_button = 0;
		break;
	case 12:
		keypad_button = '#'; // ASCII OF #
		break;
	default:
		keypad_button = button_number;
		break;
	}
	return keypad_button;
}



#elif (NUMBER_OF_COLUMNS == 4)
/*
 * This function used to adjust the buttons in 4x4 keypad to its right presentation.
 */
static uint8 KEYPAD_4x4_adjustKeyNumber(uint8 button_number)
{
	uint8 keypad_button = 0;
	switch(button_number)
	{
	case 1:
		keypad_button = 7;
		break;
	case 2:
		keypad_button = 8;
		break;
	case 3:
		keypad_button = 9;
		break;
	case 4:
		keypad_button = '%';  /* ASCII of "%" */
		break;
	case 5:
		keypad_button = 4;
		break;
	case 6:
		keypad_button = 5;
		break;
	case 7:
		keypad_button = 6;
		break;
	case 8:
		keypad_button = '*';  /* ASCII of "*" */
		break;
	case 9:
		keypad_button = 1;
		break;
	case 10:
		keypad_button = 2;
		break;
	case 11:
		keypad_button = 3;
		break;
	case 12:
		keypad_button = '-';
		break;
	case 13:
		keypad_button = 13; /* ASCII of enter */
		break;
	case 14:
		keypad_button = 0;
		break;
	case 15:
		keypad_button = '=';  /* ASCII of "=" */
		break;
	case 16:
		keypad_button = '+';  /* ASCII of "+" */
		break;
	default:
		keypad_button = button_number;
		break;
	}
	return keypad_button;
}
#endif
