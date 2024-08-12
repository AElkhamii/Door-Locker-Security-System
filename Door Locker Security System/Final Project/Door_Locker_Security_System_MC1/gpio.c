 /******************************************************************************
 *
 * Module: GPIO
 *
 * File Name: gpio.c
 *
 * Description: Source file for the AVR GPIO driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/

#include <avr/io.h> /* To use the IO Ports Registers */
#include "common_macros.h" /* To use Macros as SET_BIT */
#include "gpio.h"

/*
 * Description:
 * Setup the direction of the required pin input/output.
 * If the input port number or pin number are not correct, The function will not handle the request.
 */
void GPIO_setupPinDirection(uint8 port_num, uint8 pin_num, GPIO_PinDirectionType direction)
{
	if((port_num >= NUM_OF_PORTS) || (pin_num >= NUM_OF_PINS_PER_PORT))
	{
		/* DO NOTHING */
	}
	else
	{
		switch(port_num)
		{
		case PORTA_ID:
			if(direction == PIN_OUTPUT)
			{
				SET_BIT(DDRA,pin_num);
			}
			else
			{
				CLEAR_BIT(DDRA,pin_num);
			}
			break;
		case PORTB_ID:
			if(direction == PIN_OUTPUT)
			{
				SET_BIT(DDRB,pin_num);
			}
			else
			{
				CLEAR_BIT(DDRB,pin_num);
			}
			break;
		case PORTC_ID:
			if(direction == PIN_OUTPUT)
			{
				SET_BIT(DDRC,pin_num);
			}
			else
			{
				CLEAR_BIT(DDRC,pin_num);
			}
			break;
		case PORTD_ID:
			if(direction == PIN_OUTPUT)
			{
				SET_BIT(DDRD,pin_num);
			}
			else
			{
				CLEAR_BIT(DDRD,pin_num);
			}
			break;
		}
	}
}

/*
 *Description:
 * Write the value logic high or logic low on the required pin.
 * if the input port number or pin number are not correct, the function will not handle the request.
 * if the pin input, this function will enable/disable the internal pull-up resistor.
 */
void GPIO_writePin(uint8 port_num, uint8 pin_num, uint8 value)
{
	if((port_num >= NUM_OF_PORTS) || (pin_num >= NUM_OF_PINS_PER_PORT))
	{
		/* DO NOTHING */
	}
	else
	{
		switch(port_num)
		{
		case PORTA_ID:
			if(value == LOGIC_HIGH)
			{
				SET_BIT(PORTA,pin_num);
			}
			else
			{
				CLEAR_BIT(PORTA,pin_num);
			}
			break;
		case PORTB_ID:
			if(value == LOGIC_HIGH)
			{
				SET_BIT(PORTB,pin_num);
			}
			else
			{
				CLEAR_BIT(PORTB,pin_num);
			}
			break;
		case PORTC_ID:
			if(value == LOGIC_HIGH)
			{
				SET_BIT(PORTC,pin_num);
			}
			else
			{
				CLEAR_BIT(PORTC,pin_num);
			}
			break;
		case PORTD_ID:
			if(value == LOGIC_HIGH)
			{
				SET_BIT(PORTD,pin_num);
			}
			else
			{
				CLEAR_BIT(PORTD,pin_num);
			}
			break;
		}
	}
}

/*
 * Description:
 * Read and return the value for the  required pin, it should be logic high or logic low.
 * if the input port number or pin number are not correct, the function will return logic low.
 */
uint8 GPIO_readPin(uint8 port_num, uint8 pin_num)
{
	uint8 pin_value = LOGIC_LOW;
	if((port_num >= NUM_OF_PORTS) || (pin_num >= NUM_OF_PINS_PER_PORT))
	{
		/* DO NOTHING */
	}
	else
	{
		switch(port_num)
		{
		case PORTA_ID:
			if(BIT_IS_SET(PINA,pin_num))
			{
				pin_value = LOGIC_HIGH;
			}
			else
			{
				pin_value = LOGIC_LOW;
			}
			break;
		case PORTB_ID:
			if(BIT_IS_SET(PINB,pin_num))
			{
				pin_value = LOGIC_HIGH;
			}
			else
			{
				pin_value = LOGIC_LOW;
			}
			break;
		case PORTC_ID:
			if(BIT_IS_SET(PINC,pin_num))
			{
				pin_value = LOGIC_HIGH;
			}
			else
			{
				pin_value = LOGIC_LOW;
			}
			break;
		case PORTD_ID:
			if(BIT_IS_SET(PIND,pin_num))
			{
				pin_value = LOGIC_HIGH;
			}
			else
			{
				pin_value = LOGIC_LOW;
			}
			break;
		}
	}
	return pin_value;
}

/*
 * Description:
 * Setup the direction of the required port all pins input/output.
 * if the direction value id PORT_INPUT all pins in the port should be input pins.
 * if the direction value id PORT_OUTPUT all pins in the port should be output pins.
 * if the input port number is not correct, the function will not handle the request.
 */
void GPIO_setupPortDirection(uint8 port_num, GPIO_PortDirectionType direction)
{
	if(port_num >= NUM_OF_PORTS)
	{
		/* DO NOTHING */
	}
	else
	{
		switch(port_num)
		{
		case PORTA_ID:
			DDRA = direction;
			break;
		case PORTB_ID:
			DDRB = direction;
			break;
		case PORTC_ID:
			DDRC = direction;
			break;
		case PORTD_ID:
			DDRD = direction;
			break;
		}
	}
}

/*
 * Description:
 * Write the value of required port
 * If any pin in port is output pin the value will be written.
 * If any pin in the port is input pin this will activate/deactivate the internal pull-up resistor.
 * if the input port number is not correct, the function will not handle the request.
 */
void GPIO_writePort(uint8 port_num, uint8 value)
{
	if(port_num >= NUM_OF_PORTS)
	{
		/* Do nothing */
	}
	else
	{
		switch(port_num)
		{
		case PORTA_ID:
			PORTA = value;
			break;
		case PORTB_ID:
			PORTB = value;
			break;
		case PORTC_ID:
			PORTC = value;
			break;
		case PORTD_ID:
			PORTD = value;
			break;
		}
	}
}

/*
 * Description:
 * Read and return the value of the required port.
 * If the input port number is not correct, the function will return ZERO value.
 */
uint8 GPIO_readPort(uint8 port_num)
{
	uint8 port_value = 0;
	if(port_num >= NUM_OF_PORTS)
	{
		/* Do nothing */
	}
	else
	{
		switch(port_num)
		{
		case PORTA_ID:
			port_value = PINA;
			break;
		case PORTB_ID:
			port_value = PINB;
			break;
		case PORTC_ID:
			port_value = PINC;
			break;
		case PORTD_ID:
			port_value = PIND;
			break;
		}
	}
	return port_value;
}
