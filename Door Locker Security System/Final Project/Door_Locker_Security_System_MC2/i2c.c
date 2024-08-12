 /******************************************************************************
 *
 * Module: I2C
 *
 * File Name: i2c.c
 *
 * Description: Source file for the AVR I2C driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/

/***************************************************************************
 *  							Include Header							   *
 ***************************************************************************/
#include "i2c.h"
#include <avr/io.h>
#include "common_macros.h"

/***************************************************************************
 *                      	   Function Prototype                          *
 ***************************************************************************/
/* Description:
 * This function get the power of a number.
 * This function used in I2C_init() function to calculate 4 to the power of prescaler value.
 */
static volatile uint32 power(uint8 base, uint8 power);

/***************************************************************************
 *  						   Function Deceleration					   *
 ***************************************************************************/
/*
 * Description:
 * This function enable TWI (I2C) and give the device an address
 * It also select the prescaler for the frequency that generated from master
 */
void I2C_init(const I2C_ConfigType *config_ptr)
{
	/*
	 * Data  transfer rate = 400Kbs (F_SCL)
	 * F_SCL = F_CPU/(16 + 2* TBWR * POW(4, TWPS))
	 * TWPS0 = 0, TWPS1 = 0. Prescaler value = 1
	 * So the TWBR value will be equal to 2
	 */
	TWBR = ((F_CPU/config_ptr->mode) - 16) / (2 * power(4, config_ptr->prescaler));

	TWSR = (TWSR & 0xFC) | (config_ptr->prescaler & 0x03); /* TWPS value (Prescaler)*/

	TWCR = (1<< TWEN); /* The TWEN bit enables I2C operation and activates the I2C interface */

	TWAR = (TWAR & 0xFE) | ((1 & 0xEF)<< 1); /* Give Address number 1 to the device */
}

/*
 * Description:
 * Clear the flag and enable again the TWI
 * Activate start bit and wait for it to finish start process
 */
void I2C_start(void)
{
    /*
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * send the start bit by TWSTA=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<< TWEN);

	/* Wait for TWINT flag set in TWCR Register (start bit is send successfully) */
	while (BIT_IS_CLEAR(TWCR, TWINT));
}

/*
 * Description:
 * Clear the flag and enable again the TWI
 * Activate stop bit and wait for it to finish stop process
 */
void I2C_stop(void)
{
    /*
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * send the stop bit by TWSTO=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<< TWEN);
}

/*
 * Description:
 * Clear the flag and enable again the TWI
 * Save the required data in TWDR register and wait for it to finish this process
 */
void I2C_writeByte(uint8 data)
{
	TWDR = data; /* Save the required data in TWDR register */

    /*
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1<<TWINT) | (1<< TWEN);

	/* Wait for TWINT flag set in TWCR Register (waiting for the end of write data process) */
	while (BIT_IS_CLEAR(TWCR, TWINT));
}

/*
 * Description:
 * Clear the flag, enable again the TWI and Enable Acknowledge bit
 * Return the data from TWDR register after waiting for read process
 */
uint8 I2C_readByteWithACK(void)
{
    /*
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * Enable Acknowledge bit
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<< TWEN);

	/* Wait for TWINT flag set in TWCR Register (waiting for the end of read data process) */
	while (BIT_IS_CLEAR(TWCR, TWINT));

	return TWDR; /* Return the data */
}

/*
 * Description:
 * Clear the flag, enable again the TWI and Disable Acknowledge bit
 * Return the data from TWDR register after waiting for read process
 */
uint8 I2C_readByteWithNACK(void)
{
    /*
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * Disable Acknowledge bit
	 * Enable TWI Module TWEN=1
	 */
	TWCR = (1<<TWINT) | (1<< TWEN);

	/* Wait for TWINT flag set in TWCR Register (waiting for the end of read data process) */
	while (BIT_IS_CLEAR(TWCR, TWINT));

	return TWDR; /* Return the data */
}

/*
 * Description:
 * Read the status of the TWI logic
 */
uint8 I2C_getStatus(void)
{
	uint8 statue;
	statue = (TWSR & 0xF8); /* get only the last 5 bits */

	return statue; /* Return status value */
}


static volatile uint32 power(uint8 base, uint8 power)
{
	uint8 result = 1, i;
	for(i = 0; i < power; i++)
	{
		result = result * base;
	}
	return result;
}
