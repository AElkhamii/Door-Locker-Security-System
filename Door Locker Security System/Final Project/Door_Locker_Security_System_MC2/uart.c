/****************************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Discretion: Source file for the AVR UART driver
 *
 * Author: Abdelrahman Ehab
 *
 ****************************************************************************************/

/*******************************************************************************
 *                    	     	Include Header	                               *
 *******************************************************************************/

#include "uart.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>


/*******************************************************************************
 *                     			 Functions Definitions                         *
 *******************************************************************************/

/*
 * Description:
 * UART_init work with double baud rate and enable receive and transfer with 8-bits data.
 * bandRate ranged from 10bps to 256000bps
 */
void UART_init(uint32 a_bandRate, const UART_ConfigType *config_ptr)
{
	uint16 ubrr_value = 0;

	/*
	 * RXC, TXC, and UDRE are flag bits that set when a certain action occur.
	 * FE, DOR, and PE are flags that set when a certain error occur.
	 * U2X configured by the developer, USART Transmission Speed.
	 * MPCM = 0;
	 */
	 UCSRA = (UCSRA & 0xFD) | (config_ptr->transmissionSpeed << 1); /* transmission Speed select */

	 /*
	  * RX, TXCIE, and UDRIE are interrupt enable bits. not required since the use of polling.
	  * RXEN = 1, TXEN = 1. To enable Receiver and Transmitter.
	  * RXB8 and TXB8 not required because no need for the ninth bit.
	  * UCSZ2, configured by the developer, Character Size.
	  */
	 UCSRB |= (1<< RXEN) | (1<< TXEN);
	 UCSRB = (UCSRB & 0xFB) | ((config_ptr->CharacterSize & 0x04>>2)<<2);/* select character size */
	 UCSRB = (UCSRB & 0x7F) | (config_ptr->RXInterruptEnable<<7); /* RX Interrupt configure */
	 UCSRB = (UCSRB & 0xBF) | (config_ptr->TXInterruptEnable<<6); /* TX Interrupt configure */

	/*
	 * URSEL = 1,The URSEL must be one when writing the UCSRC.
	 * UMSEL configured by the developer, Asynchronous or  synchronous mode
	 * UPM0 , UPM1 , configured by the developer, Parity Mode.
	 * USBS configured by the developer,  stop bit = 1-bit
	 * UCSZ1, UCSZ0, configured by the developer, Character Size.
	 * UCPOL = 0, set when Asynchronous mode is used.D
	 */
	UCSRC |= (1<< URSEL);
	UCSRC = (UCSRC & 0xBF) | (config_ptr->mode << 6); /*  Asynchronous or  synchronous mode */
	/* If the developer use synchronous mode, he can configure Clock Polarity*/
	if(BIT_IS_SET(UCSRC, 6))
	{
		UCSRC = (UCSRC & 0xFE) | (config_ptr->clockPolarity << 0); /* Rising or falling edge */
	}
	UCSRC = (UCSRC & 0xCF) | (config_ptr->parityMode << 4); /* Parity Mode select */
	UCSRC = (UCSRC & 0xF7) | (config_ptr->stopBit << 3); /* One or two parity bit */
	UCSRC = (UCSRC & 0xF9) | ((config_ptr->CharacterSize & 0x03)<<1); /* select character size */


	 /* Calculate the UBRR register value */
	 ubrr_value = (uint16)((F_CPU/(a_bandRate * 8UL)) - 1);

	 /* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	 UBRRH = ubrr_value >> 8;
	 UBRRL = ubrr_value;
}


/*
 * Description:
 * wait until the UDR register is empty.
 * sent 8-bits data by put the data value in UDR register.
 */
void UART_sendByte(const uint8 data)
{
	while(BIT_IS_CLEAR(UCSRA, UDRE)){}
	UDR = data;
}

/*
 * Description:
 * wait until the UDR register receive all 8-bits data.
 * Return this data to be saved in another variable.
 */
uint8 UART_recieveByte(void)
{
	while(BIT_IS_CLEAR(UCSRA, RXC)){}
	return UDR;
}
/*
 * Description:
 * This function take a string in a pointer.
 * Then loop on each character to send each one byte by byte.
 */
void UART_sendString(const uint8* a_str_ptr)
{
	uint8 i;
	/* the pointer will point for each character and sent it individually until the pointer reach #*/
	for(i=0; *a_str_ptr != '\0'; i++)
	{
		UART_sendByte(*a_str_ptr);
		a_str_ptr++;
	}
}

/*
 * Description:
 * This function receive first byte in a string by pointer to character.
 * Then if it is not equal #, receive the next byte in the pointer++ until the value became #.
 * At the end of this string, replace the # with NULL '0\'
 */
void UART_recieveString(uint8 *a_str_ptr)
{
	uint8 i = 0;
	/* Receive the first byte */
	a_str_ptr[i] = UART_recieveByte();

	/* check on the first byte. if it is not equal # receive the next one */
	while(a_str_ptr[i] != '#')
	{
		i++;
		a_str_ptr[i] = UART_recieveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	a_str_ptr[i] = '\0';
}

