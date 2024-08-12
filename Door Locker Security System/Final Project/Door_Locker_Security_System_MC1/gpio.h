/****************************************************************************************
 *
 * Module: GPIO
 *
 * File Name: gpio.h
 *
 * Discretion: Header file for the AVR GPIO driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/


#ifndef GPIO_H_
#define GPIO_H_

/*******************************************************************
 *  						(1) Include Header					   *
 *******************************************************************/
#include "std_types.h"

/*******************************************************************
 *  						(2) Definitions						   *
 *******************************************************************/
#define NUM_OF_PORTS			4
#define NUM_OF_PINS_PER_PORT	8

#define PORTA_ID 				0
#define PORTB_ID 				1
#define PORTC_ID 				2
#define PORTD_ID 				3

#define PIN0_ID					0
#define PIN1_ID 				1
#define PIN2_ID 				2
#define PIN3_ID 				3
#define PIN4_ID 				4
#define PIN5_ID 				5
#define PIN6_ID 				6
#define PIN7_ID 				7

/*******************************************************************
 *  						(3) Type Deceleration				   *
 *******************************************************************/
typedef enum
{
	PIN_INPUT,PIN_OUTPUT
}GPIO_PinDirectionType;
typedef enum
{
	PORT_INPUT,PORT_OUTPUT=0xFF
}GPIO_PortDirectionType;

/*******************************************************************
 *  						(5) Function Prototypes				   *
 *******************************************************************/

/*
 * Description:
 * Setup the direction of the required pin input/output.
 * If the input port number or pin number are not correct, The function will not handle the request.
 */
void GPIO_setupPinDirection(uint8 port_num, uint8 pin_num, GPIO_PinDirectionType direction);

/*
 *Description:
 * Write the value logic high or logic low on the required pin.
 * if the input port number or pin number are not correct, the function will not handle the request.
 * if the pin input, this function will enable/disable the internal pull-up resistor.
 */
void GPIO_writePin(uint8 port_num, uint8 pin_num, uint8 value);

/*
 * Description:
 * Read and return the value for the  required pin, it should be logic high or logic low.
 * if the input port number or pin number are not correct, the function will return logic low.
 */
uint8 GPIO_readPin(uint8 port_num, uint8 pin_num);

/*
 * Description:
 * Setup the direction of the required port all pins input/output.
 * if the direction value id PORT_INPUT all pins in the port should be input pins.
 * if the direction value id PORT_OUTPUT all pins in the port should be output pins.
 * if the input port number is not correct, the function will not handle the request.
 */
void GPIO_setupPortDirection(uint8 port_num, GPIO_PortDirectionType direction);

/*
 * Description:
 * Write the value of required port
 * If any pin in port is output pin the value will be written.
 * If any pin in the port is input pin this will activate/deactivate the internal pull-up resistor.
 * if the input port number is not correct, the function will not handle the request.
 */
void GPIO_writePort(uint8 port_num, uint8 value);

/*
 * Description:
 * Read and return the value of the required port.
 * If the input port number is not correct, the function will return ZERO value.
 */
uint8 GPIO_readPort(uint8 port_num);

#endif /* GPIO_ */
