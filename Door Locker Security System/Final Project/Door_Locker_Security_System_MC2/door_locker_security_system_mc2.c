/*
 ================================================================================================
 Name        : door_locker_security_system_mc2.c
 Author      : Abdelrahman Ehab
 Description : A door is locked by a password.
 	 	 	   If you write the password correctly the door will open.
  	  	  	   when you enter the password 3 times wrong, the buzzer will be activated for 1 minute.
 Date        : 1/11/2021
 ================================================================================================
 */

/* Change CPU frequency to 8000000 */
#define F_CPU 8000000UL

/*******************************************************************************
 *                    	     	Include Header	                               *
 *******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "buzzer.h"
#include "dc_motor.h"
#include "external_eeprom.h"
#include "i2c.h"
#include "uart.h"
#include "timer.h"

/******************************************************************************
 *								 Definitions								  *
 ******************************************************************************/
#define BAUD 								9600 		/* Baud rate */
#define MC2_READY 							0x01 		/* Handshaking between MC1 and MC2 (if use pooling instead of interrupt in UART). */

#define EEPROM_Password_first_ADDRESS		0x0300		/* Address of first place in memory in which the first value of the password is saved. */

#define MOTOR_SPEED							75			/* it is a percentage from 0 to 100. */

#define PASSWORD_SIZE						4	 		/* To set the password size with a name. */
#define MAX_NUMBER_OF_ERRORS				3			/* This is the number of times available for the user to write the password wrong before the buzzer get activated. */

#define TIMER_OPEN_CLOSE_DOOR				233			/* This is the number of overflow required to make 15 seconds. */
#define TIMER_HOLD_DOOR						46			/* This is the number of overflow required to make 3 seconds. */
#define TIMER_BUZZER						930			/* This is the number of overflow required to make 60 seconds. */

/* Commands for making MC1 and MC2 can communicate with each other */
#define FIRST_PASSWORD						0xF1 		/* This used to inform MC2 that the password will be changed by sending a byte from MC1 with a certain value. */
#define OPEN_DOOR							0xF2		/* This used to inform MC2 that the door will be opened by sending a byte from MC1 with a certain value. */
#define OPEN_DOOR_SUCCESS					0xF3		/* To present on screen door is opening. */
#define OPEN_DOOR_FAILED					0xF4		/* To present on screen Wrong password and ask the user to repeat entering the password. */
#define CHANGE_PASSWORD						0xF5		/* This used to inform MC2 that the password will be changed by sending a byte from MC1 with a certain value. */
#define CORRECT_PASSWORD					0xF6		/* To inform MC1 that the password MC2 received is correct. */
#define WRONG_PASSWORD						0xF7		/* To inform MC1 that the password MC2 received is wrong. */
/******************************************************************************
 *							   Global Variables								  *
 ******************************************************************************/

uint16 g_timerCounter = 0;								/* To count number of overflow required to reach required time. */
uint8 g_timerFlag = 0;									/* To stop program in while loop until the timer finish counting. */

uint8 g_buzzerAccumulator = 0;							/* To make sure if the user write the password three times wrong the buzzer will be activated for 1 minute. This buzzer reset if the password is correct*/

/*******************************************************************************
 *                    	     	Function Prototype 	                           *
 *******************************************************************************/
/*
 * Description:
 * receive the password entered to open the door.
 */
void PASSWORD_receiveData(uint8 *a_passwordReceiveData_ptr);

/*
 * Description:
 * Compare the received password with the password that saved in EEPROM.
 * If True send command to display on screen door is opening and active the motor for 33 second (15 CW, 3 HOLD, 15 CCW).
 * If false accumulate a counter for the buzzer and send command incorrect password to write the password again.
 */
uint8 PASSWORD_compareFromMemory(uint8 *a_passwordReceiveData_ptr, uint8 *a_passwordSaved_ptr);

/*
 * Description;
 * Receiving the password in an array from MC1.
 * Saving the password in EEPROM.
 * Extract Password values and saves it in another array for another uses.
 */
void PASSWORD_receiveSaveMemory(uint16 eepromAddress,uint8 *passwordReceived, uint8 *passwordSaved);

/*
 * Description;
 * This function use Timer0 in AVR to delay display while opening and closing the door.
 */
void TIMER0_delayOpenClose(void);

/*
 * Description;
 * This function use Timer0 in AVR to delay display while holding the door.
 */
void TIMER0_delayHold(void);

/*
 * Description;
 * This function use Timer0 in AVR to delay display while buzzer is actiavted.
 */
void TIMER0_delayBuzzer(void);

/*******************************************************************************
 *                    	     	   Main Application                            *
 *******************************************************************************/
int main(void)
{
	uint8 passwordReceived[PASSWORD_SIZE];  			/* Receive password valued from MC1 in this array. */
	uint8 passwordSaved[PASSWORD_SIZE];					/* Extract the password values that saved in EEPROM and save it in  this array for another uses. */
	uint8 passwordReceiveData[PASSWORD_SIZE];			/* To receive the password that the user entered to open the door to compare it with the password saved in EEPROM. */

	uint8 motorStatus = FALSE;							/* To open the door or not. */
	uint8 receivedPasswordStatus = FALSE;				/* To know if the received password is correct or not. */

	/*********************************************
	 *				Drivers initiation 			 *
	 *********************************************/
	/* Enable Global Interrupt I-Bit. */
	SREG |= (1<<7);

	/* Activate Buzzer */
	BUZZER_init();

	/* Activate DC-Motor */
	DCMotor_init();

	/* Initiate timer0 configuration. */
	TIMER0_ConfigType TIMER0_config = {TIMER_OVERFLOW_MODE, OC0_DISCONNECTED, F_CPU_1024, DISABLE_CTC_INTERRUPT, ENABLE_OVF_INTERRUPT};

	/* Activate I2C with fast mode (baud rate = 400000 bps). */
	I2C_ConfigType U2C_config = {F_SCL_1, FAST_MODE}; /* I2C registers configuration. */
	I2C_init(&U2C_config);

	/* Activate UART with double speed and eight_bit character size. the baud rate = 9600 bps (using interrupt when receiving a bit). */
	UART_ConfigType UART_config = {DOUBLE_SPEED, ASYNCHRONOUS, RISING, PARITY_DISABLED, ONE_STOP_BIT, EIGHT_BIT, RX_INTERRUPT_DISABLE, TX_INTERRUPT_DISABLE}; /* UART registers configuration */
	UART_init(BAUD, &UART_config);

	_delay_ms(500);


	while(1)
	{
		/*
		 * This Switch used to switch between commands that received from MC1
		 */
		switch (UART_recieveByte())
		{
		/* Case 1: Set first password	*/
		case FIRST_PASSWORD:
			/* Receive the new password and save it in memory */
			PASSWORD_receiveSaveMemory(EEPROM_Password_first_ADDRESS, passwordReceived, passwordSaved);
			break;

		/* Case 2: Opening door	*/
		case OPEN_DOOR:

			PASSWORD_receiveData(passwordReceiveData);										/* Receive the password from MC2. */
			motorStatus = PASSWORD_compareFromMemory(passwordReceiveData, passwordSaved);	/* Check if the password is correct of not. */

			/* If the password is correct, activate the motor to open and close the door. */
			if(motorStatus == TRUE)
			{
				g_buzzerAccumulator = 0;					/* Make the buzzer counter count from 0 again to count three times after each time the password is correct. */

				UART_sendByte(OPEN_DOOR_SUCCESS);			/* Send to MC1 that the door is opening. so, display on screen this information. */

				DCMotor_rotate(CW, MOTOR_SPEED);			/* Start to rotate the motor clock wise with required speed percentage. */
				TIMER_setCallBack(TIMER0_delayOpenClose);	/* This function will call TIMER0_delayHold() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
				TIMER_init(&TIMER0_config);					/* Activate timer0 to count 15 seconds. */
				while(g_timerFlag != 1){}					/* Wait until the timer finish to continue in the code. */
				g_timerFlag = 0;							/* Always after the timer flag become become on, the developer must make it zero again for another use. */

				DCMotor_rotate(STOP, MOTOR_SPEED);			/* Stop the motor. */
				TIMER_setCallBack(TIMER0_delayHold);		/* This function will call TIMER0_delayHold() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
				TIMER_init(&TIMER0_config);					/* Activate timer0 to count 3 seconds. */
				while(g_timerFlag != 1){}					/* Wait until the timer finish to continue in the code. */
				g_timerFlag = 0;							/* Always after the timer flag become become on, the developer must make it zero again for another use. */

				DCMotor_rotate(CCW, MOTOR_SPEED);			/* Start to rotate the motor Anti-clock wise with required speed percentage. */
				TIMER_setCallBack(TIMER0_delayOpenClose);	/* This function will call TIMER0_delayOpenClose() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
				TIMER_init(&TIMER0_config);					/* Activate timer0 to count 15 seconds. */
				while(g_timerFlag != 1){}					/* Wait until the timer finish to continue in the code. */
				g_timerFlag = 0;							/* Always after the timer flag become become on, the developer must make it zero again for another use. */

				DCMotor_rotate(STOP, MOTOR_SPEED);			/* Stop the motor again. */
			}
			else if(motorStatus == FALSE)
			{
				UART_sendByte(OPEN_DOOR_FAILED);			/* Send to MC1 that the password is wrong. so, display on screen this information. */
				g_buzzerAccumulator++;						/* Increment the buzzer counter every time the user write wrong password */

				/* Check on the buzzer g_buzzerAccumulator. if not reach the maximum tries, a message will appear for a second to inform the user that he wrought a wrong password*/
				if(g_buzzerAccumulator == MAX_NUMBER_OF_ERRORS)
				{
					BUZZER_on();							/* Activate the buzzer for one minutes. */

					TIMER_setCallBack(TIMER0_delayBuzzer);  /* This function will call TIMER0_delayBuzzer() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
					TIMER_init(&TIMER0_config);				/* Activate timer0 to count 60 seconds. */
					while(g_timerFlag != 1){}				/* Wait until the timer finish to continue in the code. */
					g_timerFlag = 0;						/* Always after the timer flag become become on, the developer must make it zero again for another use. */

					BUZZER_off();							/* Deactivate the buzzer. */

					g_buzzerAccumulator = 0;				/* Make the buzzer counter count from 0 again to count three times after each time the password is correct. */
				}
			}
			break;

		/* Case 3: Change Password	*/
		case CHANGE_PASSWORD:
			PASSWORD_receiveData(passwordReceiveData);													/* Receive the password from MC2. */
			receivedPasswordStatus = PASSWORD_compareFromMemory(passwordReceiveData, passwordSaved);	/* Check if the password is correct of not. */

			/* If the password is correct, start changing the password. */
			if(receivedPasswordStatus == TRUE)
			{
				g_buzzerAccumulator = 0;					/* Make the buzzer counter count from 0 again to count three times after each time the password is correct. */

				UART_sendByte(CORRECT_PASSWORD);			/* Send to MC1 that the password is correct. so, start change the password */

				/* Receive the new password and save it in memory */
				PASSWORD_receiveSaveMemory(EEPROM_Password_first_ADDRESS, passwordReceived, passwordSaved);
			}
			else if(receivedPasswordStatus == FALSE)
			{
				UART_sendByte(WRONG_PASSWORD);				/* Send to MC1 that the password is not correct. */
				g_buzzerAccumulator++;						/* Increment the buzzer counter every time the user write wrong password */

				/* Check on the buzzer g_buzzerAccumulator. if not reach the maximum tries, a message will appear for a second to inform the user that he wrought a wrong password*/
				if(g_buzzerAccumulator == MAX_NUMBER_OF_ERRORS)
				{
					BUZZER_on();							/* Activate the buzzer for one minutes. */

					TIMER_setCallBack(TIMER0_delayBuzzer);	/* This function will call TIMER0_delayBuzzer() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
					TIMER_init(&TIMER0_config);				/* Activate timer0 to count 60 seconds. */
					while(g_timerFlag != 1){}				/* Wait until the timer finish to continue in the code. */
					g_timerFlag = 0;						/* Always after the timer flag become become on, the developer must make it zero again for another use. */

					BUZZER_off();							/* Deactivate the buzzer. */

					g_buzzerAccumulator = 0;				/* Make the buzzer counter count from 0 again to count three times after each time the password is correct. */
				}
			}
			break;
		}
	}
}



/*******************************************************************************
 *                    	     	Function Decoration                            *
 *******************************************************************************/
/*
 * Description:
 * receive the password entered to open the door.
 */
void PASSWORD_receiveData(uint8 *a_passwordReceiveData_ptr)
{
	uint8 counter = 0;
	/* Receiving all 4 password characters from MC2 after confirming it is the required password to save */
	for(counter = 0; counter < PASSWORD_SIZE; counter++)
	{
		a_passwordReceiveData_ptr[counter] = UART_recieveByte();
	}
}

/*
 * Description:
 * Compare the received password with the password that saved in EEPROM.
 * If True send command to display on screen door is opening and active the motor for 33 second (15 CW, 3 HOLD, 15 CCW).
 * If false accumulate a counter for the buzzer and send command incorrect password to write the password again.
 */
uint8 PASSWORD_compareFromMemory(uint8 *a_passwordReceiveData_ptr, uint8 *a_passwordSaved_ptr)
{
	uint8 compareCounter = 0;								/* to count values in both arrays. */
	uint8 reference = 0;									/* To take a decision according to the all values are correctly equal or not. */

	/* Compare each character from password received with password saved */
	for(compareCounter = 0; compareCounter< PASSWORD_SIZE; compareCounter++)
	{
		if(a_passwordReceiveData_ptr[compareCounter] == a_passwordSaved_ptr[compareCounter])
		{
			reference++;									/* This reference will be indicator to check if 4 characters are equal. */
		}
	}
	/* If the password is set correctly. */
	if(reference == PASSWORD_SIZE)
	{
		return TRUE;
	}

	/* Else return false to repeat the process. */
	else
	{
		return FALSE;
	}
}

/*
 * Description:
 * Receiving the password in an array from MC1.
 * Saving the password in EEPROM.
 * Extract Password values and saves it in another array for another uses.
 */
void PASSWORD_receiveSaveMemory(uint16 eepromAddress,uint8 *a_passwordReceived_ptr, uint8 *a_passwordSaved_ptr)
{
	uint8 counter = 0;
	/* Receiving all 4 password characters from MC2 after confirming it is the required password to save */
	for(counter = 0; counter < PASSWORD_SIZE; counter++)
	{
		a_passwordReceived_ptr[counter] = UART_recieveByte();
	}

	/* Saving password values in external EEPROM
	 * Extract the password values that saved in EEPROM and save it in another array
	 */
	counter = 0;
	while(counter < PASSWORD_SIZE)
	{
		EEPROM_writeByte(eepromAddress, a_passwordReceived_ptr[counter]); /* Save each character in EEPROM */
		_delay_ms(10);
		EEPROM_readByte(eepromAddress, &a_passwordSaved_ptr[counter]);	  /* Take each character from EEPROM to compare with any new password that the user is going to write on the keypad.*/
		counter++; 				/* Increment the counter */
		eepromAddress++;		/* Increment the Address */
	}
}

/*
 * Description;
 * This function use Timer0 in AVR to delay display while opening and closing the door.
 */
void TIMER0_delayOpenClose(void)
{
	g_timerCounter++;								/* For each interrupt this counter will increment by one. */

	/* Check if the timer counter reach the required limit. */
    if(g_timerCounter == TIMER_OPEN_CLOSE_DOOR)
    {
    	TIMER_deinit();								/* After the number of required interrupt is ended, the timer will stop by this function. */
    	g_timerFlag = 1;							/* Open this flag will make the delay stop, must be closed again by the developer if he wants to use the delay function again. */
        g_timerCounter = 0;							/* make timer counter zero again to be able to use it again and count up form zero. */
    }
}

/*
 * Description;
 * This function use Timer0 in AVR to delay display while holding the door.
 */
void TIMER0_delayHold(void)
{
	g_timerCounter++;								/* For each interrupt this counter will increment by one. */
    if(g_timerCounter == TIMER_HOLD_DOOR)
    {
        TIMER_deinit();								/* After the number of required interrupt is ended, the timer will stop by this function. */
    	g_timerFlag = 1;							/* Open this flag will make the delay stop, must be closed again by the developer if he wants to use the delay function again. */
        g_timerCounter = 0;							/* make timer counter zero again to be able to use it again and count up form zero. */
    }
}

/*
 * Description;
 * This function use Timer0 in AVR to delay display while buzzer is activated.
 */
void TIMER0_delayBuzzer(void)
{
	g_timerCounter++;								/* For each interrupt this counter will increment by one. */
    if(g_timerCounter == TIMER_BUZZER)
    {
        TIMER_deinit();								/* After the number of required interrupt is ended, the timer will stop by this function. */
    	g_timerFlag = 1;							/* Open this flag will make the delay stop, must be closed again by the developer if he wants to use the delay function again. */
        g_timerCounter = 0;							/* make timer counter zero again to be able to use it again and count up form zero. */
    }
}
