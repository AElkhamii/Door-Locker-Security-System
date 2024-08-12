 /******************************************************************************
 *
 * Module: I2C
 *
 * File Name: i2c.h
 *
 * Description: Header file for the AVR I2C driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/

#ifndef I2C_H_
#define I2C_H_

/***************************************************************************
 *  							Include Header							   *
 ***************************************************************************/
#include "std_types.h"

/***************************************************************************
 *                         	Types Declaration                              *
 ***************************************************************************/
typedef enum{
	F_SCL_1, F_SCL_4, F_SCL_16, F_SCL_64
}I2C_Prescaler; /* I2C prescaler bits used in bit rate formula to calculate SCL frequency */

typedef enum{
	NORMAL_MODE = 100000, FAST_MODE = 400000, FAST_MODE_PLUS = 1000000, HIGH_SPEED_MODE = 3400000
}I2C_Mode; /* Data transfer rates in I2C */

typedef struct{
	I2C_Prescaler prescaler;
	I2C_Mode mode;
}I2C_ConfigType;

/***************************************************************************
 *                      Preprocessor Macros                                *
 ***************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define I2C_START         0x08 /* start has been sent */
#define I2C_REP_START     0x10 /* repeated start */
#define I2C_MT_SLA_W_ACK  0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define I2C_MT_SLA_R_ACK  0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define I2C_MT_DATA_ACK   0x28 /* Master transmit data and ACK has been received from Slave. */
#define I2C_MR_DATA_ACK   0x50 /* Master received data and send ACK to slave. */
#define I2C_MR_DATA_NACK  0x58 /* Master received data but doesn't send ACK to slave. */

/***************************************************************************
 *  						Function Prototype							   *
 ***************************************************************************/
/*
 * Description:
 * This function enable TWI (I2C) and give the device an address
 * It also select the prescaler for the frequency that generated from master
 */
void I2C_init(const I2C_ConfigType *config_ptr);

/*
 * Description:
 * Clear the flag and enable again the TWI
 * Activate start bit and wait for it to finish start process
 */
void I2C_start(void);

/*
 * Description:
 * Clear the flag and enable again the TWI
 * Activate stop bit and wait for it to finish stop process
 */
void I2C_stop(void);
/*
 * Description:
 * Clear the flag and enable again the TWI
 * Save the required data in TWDR register and wait for it to finish this process
 */
void I2C_writeByte(uint8 data);

/*
 * Description:
 * Clear the flag, enable again the TWI and Enable Acknowledge bit
 * Return the data from TWDR register after waiting for read process
 */
uint8 I2C_readByteWithACK(void);

/*
 * Description:
 * Clear the flag, enable again the TWI and Disable Acknowledge bit
 * Return the data from TWDR register after waiting for read process
 */
uint8 I2C_readBytedWithNACK(void);

/*
 * Description:
 * Read the status of the TWI logic
 */
uint8 I2C_getStatus(void);


#endif /* I2C_H_ */
