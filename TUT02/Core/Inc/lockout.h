/*
 * lockout.h
 *
 *  Created on: Mar 23, 2023
 *      Author: rini
 */

#ifndef INC_LOCKOUT_H_
#define INC_LOCKOUT_H_

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>

void InitLockout(void *argument);

void LockoutTask1(void *argument);

void LockoutTask2(void *argument);

#endif /* INC_LOCKOUT_H_ */
