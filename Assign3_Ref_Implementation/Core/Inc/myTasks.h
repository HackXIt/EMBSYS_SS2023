/*
 * myTasks.h
 *
 *  Created on: Apr 22, 2023
 *      Author: rini
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_
#define USE_CMSIS

#include "FreeRTOS.h"
#include "usart.h"
#include "protocol.h"
#include <string.h>

#ifdef USE_CMSIS
#include "cmsis_os2.h"
#endif


#ifndef USE_CMSIS
#include "queue.h"
#endif

typedef struct queueItem {
	uint8_t taskNum;
	uint32_t value;
	// updateInterval
	// targetTask
}queueItem_t;

typedef struct producerArgument
{
	uint8_t number;
	osMessageQueueId_t monitorQueue;
	osMessageQueueId_t producerQueue;
}prodArg_t;

typedef struct monitorArgument
{
	osThreadId_t producerHandles[3];
	osMessageQueueId_t monitorQueue;
	osMessageQueueId_t producerQueue;
}monArg_t;

void StartInitializeTask(void *argument);
void ProducerTask(void *argument);
void MonitorTask(void *argument);


#endif /* INC_MYTASKS_H_ */
