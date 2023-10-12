/*
 * myTasks.h
 *
 *  Created on: Mar 18, 2023
 *      Author: rini
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_

#include "FreeRTOS.h"
#include "main.h"
#include "usart.h"
#include "cmsis_os.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ringbuffer {
	uint8_t element[10];
	uint8_t r_index;
	uint8_t w_index;
	osSemaphoreId_t sem_r;
	osSemaphoreId_t sem_w;
}ringbuffer_t;

void SemInit(uint8_t producers);
bool EnterCritical();
void taskOutput(char *buffer, uint8_t bufferSize);
bool LeaveCritical();

void Task1(void *argument);
void Task2(void *argument);

void ProducerTask(void *argument);
void ConsumerTask(void *argument);

#endif /* INC_MYTASKS_H_ */
