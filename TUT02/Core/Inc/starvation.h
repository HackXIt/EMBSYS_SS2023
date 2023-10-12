/*
 * starvation.h
 *
 *  Created on: Mar 23, 2023
 *      Author: rini
 */

#ifndef INC_STARVATION_H_
#define INC_STARVATION_H_

#include "FreeRTOS.h"
#include "cmsis_os2.h"

void InitStarvation(void *argument);

void StarvationTask1(void *argument);

void StarvationTask2(void *argument);

#endif /* INC_STARVATION_H_ */
