/*
 * myTasks.h
 *
 *  Created on: Mar 4, 2023
 *      Author: rini
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_

#include "retarget.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "main.h"

/* External Definitions for allocTask1  */
extern osThreadId_t allocTask1Handle;
extern uint32_t allocTask1Buffer[ 128 ];
extern StaticTask_t allocTask1ControlBlock;
extern const osThreadAttr_t allocTask1_attributes;

void AllocTask1(void *argument);

/* Definitions for allocTask2 */
extern osThreadId_t allocTask2Handle;
extern uint32_t allocTask2Buffer[ 128 ];
extern StaticTask_t allocTask2ControlBlock;
extern const osThreadAttr_t allocTask2_attributes;

void AllocTask2(void *argument);

/* Definitions for allocTask3 */
extern osThreadId_t allocTask3Handle;
extern uint32_t allocTask3Buffer[ 128 ];
extern StaticTask_t allocTask3ControlBlock;
extern const osThreadAttr_t allocTask3_attributes;

void AllocTask3(void *argument);

/* Definitions for pollStackSize */
extern osThreadId_t pollStackSizeHandle;
extern uint32_t pollStackSizeBuffer[ 128 ];
extern StaticTask_t pollStackSizeControlBlock;
extern const osThreadAttr_t pollStackSize_attributes;

void PollStackSize(void *argument);

/* Definitions for arbitrary functions used in tasks */

void taskInitialSpace(osThreadId_t taskHandle);

void taskInfoOutput(TaskStatus_t *taskStatus);

void heapInfoOutput(HeapStats_t *heapStats);

int factorial(int n);

int fibbonacci(int n);

void taskErrorHandler(osThreadId_t id, char* err_msg);

#endif /* INC_MYTASKS_H_ */
