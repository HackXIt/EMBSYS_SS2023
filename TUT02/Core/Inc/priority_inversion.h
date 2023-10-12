/*
 * priority_inversion.h
 *
 *  Created on: Mar 23, 2023
 *      Author: rini
 */

#ifndef INC_PRIORITY_INVERSION_H_
#define INC_PRIORITY_INVERSION_H_

#include "cmsis_os2.h"
#include "FreeRTOS.h"

void InitPrioInversion(void *argument);

void PrioInversionTask1(void *argument);
void PrioInversionTask2(void *argument);
void PrioInversionTask3(void *argument);

#endif /* INC_PRIORITY_INVERSION_H_ */
