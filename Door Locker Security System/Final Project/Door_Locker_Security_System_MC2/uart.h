/****************************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Discretion: Header file for the AVR UART driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/

#ifndef UART_H_
#define UART_H_

/*******************************************************************************
 *                    	     	Include Header	                               *
 *******************************************************************************/
#include "std_types.h"

/*******************************************************************************
 *                         	Types Declaration                                  *
 *******************************************************************************/
typedef enum{
	NORMAL_SPEED, DOUBLE_SPEED
}UART_TransmissionSpeedSelect;

typedef enum{
	ASYNCHRONOUS, SYNCHRONOUS
}UART_ModeSelect;

typedef enum{
	RISING, FALLING
}UART_ClockPolaritySelect;

typedef enum{
	PARITY_DISABLED, RESERVED, EVENPARITY, ODDPARITY
}UART_ParityModeSelect;

typedef enum{
	ONE_STOP_BIT, TWO_STOP_BIT
}UART_StopBitSelect;

typedef enum{
	FIVE_BIT, SIX_BIT, SEVEN_BIT, EIGHT_BIT, NINE_BIT = 7
}UART_CharacterSizeSelect;
typedef enum{
	RX_INTERRUPT_DISABLE, RX_INTERRUPT_ENABLE
}UART_RX_Interrupt_Enable;
typedef enum{
	TX_INTERRUPT_DISABLE, TX_INTERRUPT_ENABLE
}UART_TX_Interrupt_Enable;

typedef struct{
	UART_TransmissionSpeedSelect transmissionSpeed;
	UART_ModeSelect mode;
	UART_ClockPolaritySelect clockPolarity;
	UART_ParityModeSelect parityMode;
	UART_StopBitSelect stopBit;
	UART_CharacterSizeSelect CharacterSize;
	UART_RX_Interrupt_Enable RXInterruptEnable;
	UART_TX_Interrupt_Enable TXInterruptEnable;
}UART_ConfigType;

/*******************************************************************************
 *                    	  External Public Global Variables        	           *
 *******************************************************************************/

/*******************************************************************************
 *                         	Function Prototypes                                *
 *******************************************************************************/

/*
 * Description:
 * UART_init work with double baud rate and enable receive and transfer with 8-bits data.
 * bandRate ranged from 2400bps to 0.5 Mbps
 */
void UART_init(uint32 a_bandRate, const UART_ConfigType *config_ptr);

/*
 * Description:
 * wait until the UDR register is empty.
 * sent 8-bits data by put the data value in UDR register.
 */
void UART_sendByte(const uint8 data);

/*
 * Description:
 * wait until the UDR register receive all 8-bits data.
 * Return this data to be saved in another variable.
 */
uint8 UART_recieveByte(void);

/*
 * Description:
 * This function take a string in a pointer.
 * Then loop on each character to send each one byte by byte.
 */
void UART_sendString(const uint8* a_str_ptr);

/*
 * Description:
 * This function receive first byte in a string by pointer to character.
 * Then if it is not equal #, receive the next byte in the pointer++ until the value became #.
 * At the end of this string, replace the # with NULL '0\'
 */
void UART_recieveString(uint8* a_str_ptr);

#endif /* UART_H_ */
