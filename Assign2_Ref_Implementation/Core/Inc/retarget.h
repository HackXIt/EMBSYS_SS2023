/**
 * @file 	retarget.h
 * @author	Clemens Koernyefalvy
 * @date	27/02/2023
 * @version	1.0
 * @brief	Header file for the retarget.c implementation.
 */
#ifndef INC_RETARGET_H_
#define INC_RETARGET_H_

/**************** INCLUDES ****************/
#include <stdio.h>			// for retargeting the standard printf() function
#include "stm32l4xx_hal.h" 	// for UART_HandleTypeDef type

/**************** GLOBALS ****************/
extern UART_HandleTypeDef huart2;

#endif /* INC_RETARGET_H_ */
