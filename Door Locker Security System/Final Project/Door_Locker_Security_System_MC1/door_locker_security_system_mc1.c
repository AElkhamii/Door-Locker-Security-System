/*
 ================================================================================================
 Name        : door_locker_security_system_mc1.c
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
#include "keypad.h"
#include "lcd.h"
#include "uart.h"
#include "timer.h"

/*******************************************************************************
 *                    	     	   Definitions 	                               *
 *******************************************************************************/
#define BAUD 								9600 		/* Baud rate */
#define MC2_READY 							0x01 		/* Handshaking between MC1 and MC2 (if use pooling instead of interrupt in UART). */

#define ENTER  								13   		/* For the enter button. */

#define PASSWORD_SIZE						4	 		/* To set the password size with a name. */
#define MAX_NUMBER_OF_ERRORS				3			/* This is the number of times available for the user to write the password wrong before the buzzer get activated. */

#define TIMER_OPEN_CLOSE_DOOR				233	 		/* This is the number of overflow required to make 15 seconds. */
#define TIMER_HOLD_DOOR						46			/* This is the number of overflow required to make 3 seconds. */
#define TIMER_BUZZER						930			/* This is the number of overflow required to make 60 seconds. */

/* Commands for making MC1 and MC2 can communicate with each other */
#define FIRST_PASSWORD						0xF1 		/* This used to inform MC2 that the first password will be created by sending password data for the first time to MC2 from MC1. */
#define OPEN_DOOR							0xF2		/* This used to inform MC2 that the door will be opened by sending a byte from MC1 with a certain value. */
#define OPEN_DOOR_SCREEN					0xF3		/* To present on screen door is opening. */
#define OPEN_DOOR_SUCCESS					0xF3		/* To present on screen door is opening. */
#define OPEN_DOOR_FAILED					0xF4		/* To present on screen Wrong password and ask the user to repeat entering the password. */
#define CHANGE_PASSWORD						0xF5		/* This used to inform MC2 that the password will be changed by sending a byte from MC1 with a certain value. */
#define CORRECT_PASSWORD					0xF6		/* To inform MC1 that the password MC2 received is correct */
#define WRONG_PASSWORD						0xF7		/* To inform MC1 that the password MC2 received is wrong */
/******************************************************************************
 *							   Global Variables								  *
 ******************************************************************************/

uint16 g_timerCounter = 0;					/* To count number of overflow required to reach required time. */
uint8 g_timerFlag = 0;						/* To stop program in while loop until the timer finish counting. */

uint8 g_buzzerAccumulator = 0;				/* To make sure if the user write the password three times wrong the buzzer will be activated for 1 minute. This buzzer reset if the password is correct. */


/*******************************************************************************
 *                    	     	Function Prototype 	                           *
 *******************************************************************************/
/*
 * Description:
 * Get from the user the values that he entered on the keypad.
 */
void PASSWORD_getData(uint8 *a_passwordEnterData_ptr);

/*
 * Description:
 * send password that the user entered to MC2 to check if the password is correct.
 */
void PASSWORD_sendData(uint8 *a_passwordEnterData_ptr);

/*
 * Description:
 * This function points to two arrays, and save to them the values of the passwords that the user write from the keypad.
 */
void PASSWORD_saveData(uint8 *a_passwordFirstSave_ptr, uint8 *a_passwordSecondSave_ptr);

/*
 * Description:
 * This function used to check if both first and second passwords are equal or not.
 * If the password is set correctly, start to send the password by UART to the MC2.
 * Else repeat the process.
 */
uint8 PASSWORD_compareFirstSecondValues(uint8 *a_passwordFirstTime_ptr, uint8 *a_passwordSecondTime_ptr);


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
 * This function use Timer0 in AVR to delay display while buzzer is activated.
 */
void TIMER0_delayBuzzer(void);


/*******************************************************************************
 *                    	     	   Main Application                            *
 *******************************************************************************/
int main(void)
{
	uint8 passwordFirstSave[PASSWORD_SIZE]; 	/* Array for the first password. */
	uint8 passwordSecondSave[PASSWORD_SIZE];	/* Array for the Repeated password. */
	uint8 passwordEnterData[PASSWORD_SIZE];		/* Array to save the values of the password that the user will provide to open the door. */

	uint8 optionsStatus = 0;					/* To save the value that came from keypad to select from the options. */

	uint8 passwordCompareResult = FALSE;		/* To hold the the value of TRUE or FALSE to confirm if the password is saved correctly or not. */
	uint8 receivedPasswordStatus = FALSE;		/* To know if the received password is correct or not. */
	uint8 passwordStatus = 0;					/* To know from MC2 if the password that send from MC1 is correct or not. */
	uint8 buzzerStatus	 = 0;					/* To know from MC2 if the buzzer is active or not. */

	/*********************************************
	 *				Drivers initiation 			 *
	 *********************************************/
	/* Enable Global Interrupt I-Bit. */
	SREG |= (1<<7);

	/* Activate LCD */
	LCD_init();

	/* Initiate timer0 configuration. */
	TIMER0_ConfigType TIMER0_config = {TIMER_OVERFLOW_MODE, OC0_DISCONNECTED, F_CPU_1024, DISABLE_CTC_INTERRUPT, ENABLE_OVF_INTERRUPT};

	/* Activate UART with double speed and eight_bit character size. the baud rate = 9600 bps (using interrupt when receiving a bit). */
	UART_ConfigType UART_config = {DOUBLE_SPEED, ASYNCHRONOUS, RISING, PARITY_DISABLED, ONE_STOP_BIT, EIGHT_BIT, RX_INTERRUPT_DISABLE, TX_INTERRUPT_DISABLE}; /* UART registers configuration */
	UART_init(BAUD, &UART_config);

	/*********************************************
	 *	 Save The Password For The First Time	 *
	 *********************************************/
	/* Check each time if the  repeated password is not correct repeat the process. */
	while(passwordCompareResult != TRUE)
	{
		PASSWORD_saveData(passwordFirstSave, passwordSecondSave);										  /* Get password values from the user */
		passwordCompareResult = PASSWORD_compareFirstSecondValues(passwordFirstSave, passwordSecondSave); /* Check if the password is correct or not */
		/* Check if the repeated password is correct or not, if correct send it to MC2 to save it in EEPROM. */
		if(passwordCompareResult == TRUE)
		{
			UART_sendByte(FIRST_PASSWORD); 			/* Send Change command Password to the MC2 */
			PASSWORD_sendData(passwordFirstSave);	/* Send Password */
		}

		/* If the repeated password is not correct the process will be repeated. */
		else if(passwordCompareResult == FALSE)
		{
			/* in case of wrong password, inform the user he repeated the password wrongly. So, the process must be repeated. */
			LCD_clearScreen();	/* Clear the screen to present new statement on it. */
			LCD_displayStringRowColumn(0, 0, "Repeated Password");
			LCD_displayStringRowColumn(1, 4, "is Wrong");
			_delay_ms(1000);
			LCD_clearScreen();	/* Clear the screen to present new statement on it. */
			LCD_displayStringRowColumn(0, 3, "Repeat the");
			LCD_displayStringRowColumn(1, 4, "Process");
			_delay_ms(1000);
		}
		LCD_clearScreen(); /* Clear the screen to present new statement on it. */
	}

	/* Always after using PASSWORD_compareFirstSecondValues() function make sure to return  this variable to false state to use it another timer for another comparison. */
	passwordCompareResult = FALSE;

	/* Present on screen the option available  to use by the user. */
	LCD_clearScreen();	/* Clear the screen to present new statement on it. */
	LCD_displayStringRowColumn(0, 0, "+: Open Door");
	LCD_displayStringRowColumn(1, 0, "-: Change Pass");

	while(1)
	{
		optionsStatus = KEYPAD_getPressedKey();

		/*********************************************
		 *	When user select option (+) from keypad  *
		 *********************************************/
		if(optionsStatus == '+')
		{
			UART_sendByte(OPEN_DOOR); 					 /* Send command to MC2 to open the door. */

			PASSWORD_getData(passwordEnterData);		 /* Get the password from the user by using the keypad. */
			PASSWORD_sendData(passwordEnterData);		 /* Send the password to MC2. */

			passwordStatus = UART_recieveByte();		 /* Receive from MC2 the status of the password if it is correct or wrong. */

			/* If the password is correct. */
			if(passwordStatus == OPEN_DOOR_SUCCESS)
			{
				g_buzzerAccumulator = 0;				 /* Make the buzzer counter count from 0 again to count three times after each time the password is correct. */

				LCD_clearScreen();						 /* Clear the screen to present new statement on it. */
				LCD_displayString("Opening the door");	 /* Present opening the door while the motor is rotating clockwise. */
				TIMER_setCallBack(TIMER0_delayOpenClose);/* This function will call TIMER0_delayOpenClose() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
				TIMER_init(&TIMER0_config);				 /* Activate timer0 to count 15 seconds. */
				while(g_timerFlag != 1){}				 /* Wait until the timer finish to continue in the code. */
				g_timerFlag = 0;						 /* Always after the timer flag become become on, the developer must make it zero again for another use. */

				LCD_clearScreen();						 /* Clear the screen to present new statement on it. */
				LCD_displayString("Holding the door");	 /* Present holding the door while the motor is in holding condition.*/
				TIMER_setCallBack(TIMER0_delayHold);	 /* This function will call TIMER0_delayHold() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
				TIMER_init(&TIMER0_config);				 /* Activate timer0 to count 3 seconds. */
				while(g_timerFlag != 1){}				 /* Wait until the timer finish to continue in the code. */
				g_timerFlag = 0;						 /* Always after the timer flag become become on, the developer must make it zero again for another use. */

				LCD_clearScreen();						 /* Clear the screen to present new statement on it. */
				LCD_displayString("Closing the door");	 /* Present closing the door while the motor is rotating Anti-clockwise.*/
				TIMER_setCallBack(TIMER0_delayOpenClose);/* This function will call TIMER0_delayOpenClose() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
				TIMER_init(&TIMER0_config);				 /* Activate timer0 to count 15 seconds. */
				while(g_timerFlag != 1){}				 /* Wait until the timer finish to continue in the code. */
				g_timerFlag = 0;						 /* Always after the timer flag become become on, the developer must make it zero again for another use. */
			}

			/* If the password is not correct. */
			else if(passwordStatus ==  OPEN_DOOR_FAILED)
			{
				g_buzzerAccumulator++;					 /* Increment the buzzer counter every time the user write wrong password */

				LCD_clearScreen();						 /* Clear the screen to present new statement on it. */

				/* Check on the buzzer g_buzzerAccumulator. if not reach the maximum tries, a message will appear for a second to inform the user that he wrought a wrong password*/
				if(g_buzzerAccumulator != MAX_NUMBER_OF_ERRORS)
				{
					LCD_displayString("Wrong Password"); /* Inform the user that he wrought a wrong password. */
					_delay_ms(500);
				}

				/* If the buzzer counter reach the maximum number of tries, the buzzer will be activated for one minute. */
				if(g_buzzerAccumulator == MAX_NUMBER_OF_ERRORS)
				{
					LCD_clearScreen();					 /* Clear the screen to present new statement on it. */
					LCD_displayStringRowColumn(0, 5, "ERROR!!"); /* Inform the user that an error has occurred due to he wrought the password many times wrong. */

					TIMER_setCallBack(TIMER0_delayBuzzer);/* This function will call TIMER0_delayBuzzer() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
					TIMER_init(&TIMER0_config);			 /* Activate timer0 to count 60 seconds. */
					while(g_timerFlag != 1){}			 /* Wait until the timer finish to continue in the code. */
					g_timerFlag = 0;					 /* Always after the timer flag become become on, the developer must make it zero again for another use. */

					g_buzzerAccumulator = 0;			 /* Make the buzzer accumulator zero again to repeat the process of waiting from the user to write the password wrong three times. */
				}
			}

			/* Present on screen the option available  to use by the user. */
			LCD_clearScreen();							 /* Clear the screen to present new statement on it. */
			LCD_displayStringRowColumn(0, 0, "+: Open Door");
			LCD_displayStringRowColumn(1, 0, "-: Change Pass");
		}

		/*********************************************
		 *	When user select option (-) from keypad  *
		 *********************************************/
		else if(optionsStatus == '-')
		{
			UART_sendByte(CHANGE_PASSWORD);				 /* Send command to MC2 to change the password. */

			PASSWORD_getData(passwordEnterData);		 /* Get the password from the user by using the keypad. */
			PASSWORD_sendData(passwordEnterData);		 /* Send the password to MC2. */

			receivedPasswordStatus = UART_recieveByte(); /* Receive from MC2 the status of the password if it is correct or wrong. */

			/* If the password is correct. */
			if(receivedPasswordStatus == CORRECT_PASSWORD)
			{
				g_buzzerAccumulator = 0;				 /* Make the buzzer counter count from 0 again to count three times after each time the password is correct. */

				/* Check each time if the  repeated password is not correct repeat the process. */
				while(passwordCompareResult != TRUE)
				{
					PASSWORD_saveData(passwordFirstSave, passwordSecondSave); 										  /* Get password values from the user. */
					passwordCompareResult = PASSWORD_compareFirstSecondValues(passwordFirstSave, passwordSecondSave); /* Check if the password is correct or not. */

					/* Check each time if the  repeated password is not correct repeat the process. */
					if(passwordCompareResult == TRUE)
					{
						PASSWORD_sendData(passwordFirstSave);/* Send Password */
					}

					/* If the repeated password is not correct the process will be repeated. */
					if(passwordCompareResult == FALSE)
					{
						/* in case of wrong password, inform the user he repeated the password wrongly and the process must be repeated. */
						LCD_clearScreen();	/* Clear the screen to present new statement on it */
						LCD_displayStringRowColumn(0, 0, "Repeated Password");
						LCD_displayStringRowColumn(1, 4, "is Wrong");
						_delay_ms(1000);
						LCD_clearScreen();				 /* Clear the screen to present new statement on it. */
						LCD_displayStringRowColumn(0, 3, "Repeat the");
						LCD_displayStringRowColumn(1, 4, "Process");
						_delay_ms(1000);
					}
					LCD_clearScreen();					 /* Clear the screen to present new statement on it. */
				}

				/* Always after using PASSWORD_compareFirstSecondValues() function make sure to return  this variable to false state to use it another timer for another comparison. */
				passwordCompareResult = FALSE;
			}

			/* If the password is not correct. */
			else if(receivedPasswordStatus == WRONG_PASSWORD)
			{
				g_buzzerAccumulator++;					 /* Increment the buzzer counter every time the user write wrong password */

				LCD_clearScreen();						 /* Clear the screen to present new statement on it. */

				/* Check on the buzzer g_buzzerAccumulator. if not reach the maximum tries, a message will appear for a second to inform the user that he wrought a wrong password*/
				if(g_buzzerAccumulator != MAX_NUMBER_OF_ERRORS)
				{
					LCD_displayString("Wrong Password"); /* Inform the user that he wrought a wrong password. */
					_delay_ms(500);
				}

				/* If the buzzer counter reach the maximum number of tries, the buzzer will be activated for one minute. */
				if(g_buzzerAccumulator == MAX_NUMBER_OF_ERRORS)
				{
					LCD_clearScreen();					 /* Clear the screen to present new statement on it. */
					LCD_displayStringRowColumn(0, 5, "ERROR!!"); /* Inform the user that an error has occurred due to he wrought the password many times wrong. */

					TIMER_setCallBack(TIMER0_delayBuzzer); /* This function will call TIMER0_delayBuzzer() function when timer0 finish counting. When this function is called the Timer0 will deactivated. */
					TIMER_init(&TIMER0_config);			 /* Activate timer0 to count 60 seconds. */
					while(g_timerFlag != 1){}			 /* Wait until the timer finish to continue in the code. */
					g_timerFlag = 0;					 /* Always after the timer flag become become on, the developer must make it zero again for another use. */

					g_buzzerAccumulator = 0;			 /* Make the buzzer counter count from 0 again to count three times after each time the password is correct. */
				}
			}

			/* Present on screen the option available  to use by the user. */
			LCD_clearScreen();							 /* Clear the screen to present new statement on it. */
			LCD_displayStringRowColumn(0, 0, "+: Open Door");
			LCD_displayStringRowColumn(1, 0, "-: Change Pass");
		}
	}
}




/*******************************************************************************
 *                    	     	Function Decoration                            *
 *******************************************************************************/

/*
 * Description:
 * Get from the user the values that he entered on the keypad and send it to MC2 to check if the password is correct.
 */
void PASSWORD_getData(uint8 *a_passwordEnterData_ptr)
{
	uint8 passwordCounter = 0; 				/* To count the loop to get the values of the password. */

	LCD_clearScreen();	/* Clear the screen to present new statement on it. */
	LCD_displayStringRowColumn(0, 0, "Enter Password:");	/* Display on LCD (Enter Password:). */
	LCD_moveCursor(1, 0);									/* Move the cursor to the second line. */

	/* This loop to get password values from the user */
	while((passwordCounter < PASSWORD_SIZE))
	{
		a_passwordEnterData_ptr[passwordCounter] = KEYPAD_getPressedKey(); /* Get each keypad input and save its value in a variable from the array. */

		/*	Check if the input not a number from the keypad, repeat the loop until it get a number.	*/
		if((a_passwordEnterData_ptr[passwordCounter]<0) || (a_passwordEnterData_ptr[passwordCounter]>9))
		{
			continue;
		}

		LCD_displayCharacter('*'); 			/* For each input form the keypad, the LCD will display (*). */
		_delay_ms(500); 					/* delay for 0.5 second between each input from the keypad. */

		passwordCounter++; 					/* Increment to the next variable in the array. */
	}

	/* Waiting from user to press enter. */
	while(KEYPAD_getPressedKey() != ENTER){};
}

/*
 * Description:
 * send password that the user entered to MC2 to check if the password is correct.
 */
void PASSWORD_sendData(uint8 *a_passwordEnterData_ptr)
{
	uint8 passwordCounter = 0; 				/* To count the loop to send the values of the password. */

	/* Loop on each character from the password and send it to MC2 character by character. */
	for(passwordCounter = 0; passwordCounter < PASSWORD_SIZE; passwordCounter++)
	{
		UART_sendByte(a_passwordEnterData_ptr[passwordCounter]); /* Send all 4 password Numbers to MC2. */
	}
}

/*
 * Description:
 * This function points to two arrays, and save to them the values of the passwords that the user write from the keypad.
 */
void PASSWORD_saveData(uint8 *a_passwordFirstSave_ptr, uint8 *a_passwordSecondSave_ptr)
{
	uint8 passwordFirstSaveCounter = 0; 	/* To count for the first array to save the first input password. */
	uint8 passwordSecondSaveCounter = 0;	/* To count for the second array to save the second input password. */

	LCD_clearScreen();										/* Clear the screen to present new statement on it. */
	LCD_displayStringRowColumn(0, 0, "Save Password:"); 	/* Display on LCD (Save Password:). */
	LCD_moveCursor(1, 0);									/* Move the cursor to the second line. */

	/* This loop to save first input password. */
	while((passwordFirstSaveCounter < PASSWORD_SIZE))
	{
		a_passwordFirstSave_ptr[passwordFirstSaveCounter] = KEYPAD_getPressedKey();		/* Get each keypad input and save its value in a variable from the array. */

		/*	Check if the input not a number from the keypad, repeat the loop until it get a number. */
		if((a_passwordFirstSave_ptr[passwordFirstSaveCounter]<0) || (a_passwordFirstSave_ptr[passwordFirstSaveCounter]>9))
		{
			continue;
		}

		LCD_displayCharacter('*'); 		/* For each input form the keypad, the LCD will display (*). */
		_delay_ms(500); 				/* delay for 0.5 second between each input from the keypad. */

		passwordFirstSaveCounter++; 	/* Increment to the next variable in the array. */
	}

	/* Waiting from user to press enter */
	while(KEYPAD_getPressedKey() != ENTER){};

	/* clear the screen and present on screen (Repeat Password). */
	LCD_clearScreen();					/* Clear the screen to present new statement on it. */
	LCD_displayStringRowColumn(0, 0, "Repeat Password:");
	LCD_moveCursor(1, 0);

	/* This loop to save second input password */
	while((passwordSecondSaveCounter < PASSWORD_SIZE))
	{
		a_passwordSecondSave_ptr[passwordSecondSaveCounter] = KEYPAD_getPressedKey();	/* Get each keypad input and save its value in a variable from the array. */

		/*	Check if the input not a number from the keypad, repeat the loop until it get a number.	*/
		if((a_passwordSecondSave_ptr[passwordSecondSaveCounter]<0) || (a_passwordSecondSave_ptr[passwordSecondSaveCounter]>9))
		{
			continue;
		}

		LCD_displayCharacter('*');   		/* For each input form the keypad, the LCD will display (*). */
		_delay_ms(500); 			 		/* delay for 0.1 second between each input from the keypad. */

		passwordSecondSaveCounter++; 		/* Increment to the next variable in the array. */
	}

	/* Waiting from user to press enter. */
	while(KEYPAD_getPressedKey() != ENTER){};
}

/*
 * Description:
 * This function used to check if both first and second passwords are equal.
 * If the password is set correctly, start to send the password by UART to the MC2.
 * Else repeat the process.
 */
uint8 PASSWORD_compareFirstSecondValues(uint8 *a_passwordFirstTime_ptr, uint8 *a_passwordSecondTime_ptr)
{
	uint8 counter 	= 0; 			/* to count values in both arrays. */
	uint8 reference = 0;			/* To take a decision according to the all values are correctly equal or not. */

	/* Check if both first and second passwords are equal.  */
	while(counter < PASSWORD_SIZE)
	{
		if(a_passwordFirstTime_ptr[counter] == a_passwordSecondTime_ptr[counter])
		{
			reference++; 			/* This reference will be indicator to check if 4 characters are equal. */
		}
		counter++;
	}

	/* If the password is set correctly, start to send the password by UART to the MC2. */
	if(reference == PASSWORD_SIZE)
	{
		return TRUE;
	}

	/* Return false to repeat the process. */
	else
	{
		return FALSE;
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
