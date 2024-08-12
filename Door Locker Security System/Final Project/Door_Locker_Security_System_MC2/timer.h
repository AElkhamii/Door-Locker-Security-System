 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.h
 *
 * Description: Header file for the AVR Timer driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

/******************************************************************************
 *                    	     	Include Header	                              *
 ******************************************************************************/
#include "std_types.h"

/******************************************************************************
 *                         	   Types Declaration                              *
 ******************************************************************************/

/******************************************************************************
* 							  Timer0 Types Declaration 				    	  *
*******************************************************************************/
typedef enum{
	TIMER_OVERFLOW_MODE, TIMER_CTC_MODE = 2
}TIMER_WaveGenerationMode;

typedef enum{
	 OC0_DISCONNECTED, OC0_TOGGLE, OC0_CLEAR, OCO_SET
}TIMER_CompareMatchOutputMode;

typedef enum{
	 NO_CLK, F_CPU_0, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024, EXTERNAL_CLK_FALLING_EDGE, EXTERNAL_CLK_RISING_EDGE
}TIMER_Prescaler;

typedef enum{
	 DISABLE_CTC_INTERRUPT, ENABLE_CTC_INTERRUPT
}TIMER_CTC_Interrupt;

typedef enum{
	 DISABLE_OVF_INTERRUPT, ENABLE_OVF_INTERRUPT
}TIMER_Overflow_Interrupt;

typedef struct{
	TIMER_WaveGenerationMode waveGenerationMode;
	TIMER_CompareMatchOutputMode compareMatchOutputMode;
	TIMER_Prescaler prescaler;
	TIMER_CTC_Interrupt CTC_Interrupt;
	TIMER_Overflow_Interrupt OVERFLOW_Interrupt;

	uint16 CTC_VALUE; /* To set CTC value in OCR0 register */
}TIMER0_ConfigType;

/******************************************************************************
 *                         	   Function Prototypes                            *
 ******************************************************************************/
/*
 * Description:
 * Initiate the timer with any mode required (CTC OR Normal) mode with required frequency.
 */
void TIMER_init(TIMER0_ConfigType *config_ptr);

/*
 * Description:
 * This function will call a required function to do a cretin thing when the timer finish counting.
 */
void TIMER_setCallBack(void(*a_ptr)(void));

/*
 * Description:
 * Deactivate all registers in the timer
 */
void TIMER_deinit(void);

#endif /* TIMER_H_ */
