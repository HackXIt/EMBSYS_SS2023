/*
 * deadlock.h
 *
 *  Created on: Mar 23, 2023
 *      Author: rini
 */

#ifndef INC_DEADLOCK_H_
#define INC_DEADLOCK_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "usart.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

void InitDeadlock(void *argument);

void DeadlockTask1(void *argument);

void DeadlockTask2(void *argument);

#endif /* INC_DEADLOCK_H_ */
