 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Source file for the AVR Timer driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/


/******************************************************************************
 *                    	     	Include Header	                              *
 ******************************************************************************/
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "common_macros.h"
#include "gpio.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;


/******************************************************************************
 *                         	   Function Declaration                            *
 ******************************************************************************/
/*
 * Description:
 * Initiate the timer with any mode required (CTC OR Normal) mode with required frequency.
 */
void TIMER_init(TIMER0_ConfigType *config_ptr)
{
	/*************************************************************************
	 								Timer0
	 *************************************************************************/
	TCCR0 |= (1<< FOC0); /* The FOC0 bit is only active when the WGM00 bit specifies a non-PWM mode */
	/* Select wave generation mode */
	TCCR0 = (TCCR0 & 0xB7) | ((((config_ptr->waveGenerationMode & 0x02)>>1)<<6) | ((config_ptr->waveGenerationMode & 0x01)<<3));
	/* Select Compare Match Output Mode*/
	TCCR0 = (TCCR0 & 0xCF) | ((config_ptr->compareMatchOutputMode & 0x03)<<4);

	/* Select prescaler */
	TCCR0 = (TCCR0 & 0xF8) | ((config_ptr->prescaler & 0x07)<<0);

	TCNT0 = 0;     					/*Set Timer initial value to 0*/
	OCR0  = config_ptr->CTC_VALUE;  /*Set Compare Value*/
	TIMSK = (TIMSK & 0xFE) | ((config_ptr->OVERFLOW_Interrupt & 0x01)<<0); /*Enable Timer Overflow Interrupt*/
	TIMSK = (TIMSK & 0xFD) | ((config_ptr->CTC_Interrupt & 0x01)<<1);   /*Enable Timer Compare Interrupt*/

}

/*
 * Description:
 * This function will call a required function to do a cretin thing when the timer finish counting.
 */
void TIMER_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

/*
 * Description:
 * Deactivate all registers in the timer
 */
void TIMER_deinit(void)
{
	/*************************************************************************
	  						Clear All Timer0 Registers
	 *************************************************************************/
	TCCR0 = 0;
	TCNT0 = 0;
	OCR0 = 0;
	/* Disable interrupt for both normal and compare mode */
	TIMSK &= ~(1<< TOIE0) & (1<< OCIE0);;
}

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the overflow occur in timer0 */
		(*g_callBackPtr)();
	}
}

ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the compare occur in timer0*/
		(*g_callBackPtr)();
	}
}

