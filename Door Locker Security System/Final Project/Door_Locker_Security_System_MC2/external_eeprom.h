 /******************************************************************************
 *
 * Module: External EEProm
 *
 * File Name: external_eeprom.h
 *
 * Description: Header file for the AVR External EEProm driver
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/

#ifndef EXTERNAL_EEPROM_H_
#define EXTERNAL_EEPROM_H_

/***************************************************************************
 *  							Include Header							   *
 ***************************************************************************/
#include "std_types.h"

/***************************************************************************
 *                      Preprocessor Macros                                *
 ***************************************************************************/
#define ERROR 0
#define SUCCESS 1

/***************************************************************************
 *  							Function Prototype						   *
 ***************************************************************************/
/*
 * Description:
 * Save a value in memory.
 */
uint8 EEPROM_writeByte(uint16 u16addr, uint8 u8data);

/*
 * Description:
 * Read value from memory.
 */
uint8 EEPROM_readByte(uint16 u16addr, uint8 *u8data);

#endif /* EXTERNAL_EEPROM_H_ */
