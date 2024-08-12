/*******************************************************************************
 *
 * Module: Common Macros
 *
 * File Name: common_macros.h
 *
 * Description: commonly used macros
 *
 * Author: Abdelrahman Ehab
 *
 *******************************************************************************/


#ifndef COMMON_MACROS_
#define COMMON_MACROS_

/* Set a certain bit in any register */
#define SET_BIT(REG,BIT_NUM) (REG |= (1<<BIT_NUM))

/* Clear a certain bit in any register */
#define CLEAR_BIT(REG,BIT_NUM) (REG &= ~(1<<BIT_NUM))

/* Toggle a certain bit in any register */
#define TOGGLE_BIT(REG,BIT_NUM) (REG ^= (1<<BIT_NUM))

/* Rotate right the register value with specific number of rotates */
#define ROTATE_RIGHT(REG,NUM) (REG = (REG>>NUM) | (REG<<(8-NUM>)))

/* Rotate left the register value with specific number of rotates */
#define ROTATE_LEFT(REG,NUM) (REG = (REG<<NUM) | (REG>>(8-NNUM)))

/* Check if a specific bit is set in any register and return true if yes */
#define BIT_IS_SET(REG,BIT_NUM) (REG & (1<<BIT_NUM))

/* Check if a specific bit is cleared in any register and return true if yes */
#define BIT_IS_CLEAR(REG,BIT_NUM) (!(REG & (1<<BIT_NUM)))

/* Get the value of a certain bit*/
#define GET_BIT(REG,BIT_NUM) ((REG & (1<<BIT_NUM))>>BIT_NUM)


#endif
