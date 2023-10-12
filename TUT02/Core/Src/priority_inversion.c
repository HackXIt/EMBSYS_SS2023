/*
 * priority_inversion.c
 *
 *  Created on: Mar 23, 2023
 *      Author: rini
 */

#include "priority_inversion.h"

//#define USE_MUTEX

#ifdef USE_MUTEX
osMutexId_t myMutex;
const osMutexAttr_t myMutex_attr = {
		.name = "MyMutex",
		// Available bit masks: osMutexRecursive, osMutexPrioInherit, osMutexRobust
		.attr_bits = 0 // Priority ceiling: set this to osMutexPrioInherit
};
#endif

#ifndef USE_MUTEX
// Semaphore do not have priority ceiling functionality
// the functionality must be implemented by the user
osSemaphoreId_t mySemaphore;
#endif

void InitPrioInversion(void *argument) {
	/* Definitions for blinkThread and UARTThread */
	osThreadId_t prioTask1;
	const osThreadAttr_t prioTask1_attr = {
			.name = "Prio1",
			.stack_size = 128 * 4, /* In bytes */
			.priority = (osPriority_t) osPriorityLow,
	};
	osThreadId_t prioTask2;
	const osThreadAttr_t prioTask2_attr = {
			.name = "Prio2",
			.stack_size = 128 * 4, /* In bytes */
			.priority = (osPriority_t) osPriorityNormal,
	};
	osThreadId_t prioTask3;
	const osThreadAttr_t prioTask3_attr = {
			.name = "Prio3",
			.stack_size = 128 * 4, /* In bytes */
			.priority = (osPriority_t) osPriorityAboveNormal,
	};
	prioTask1 = osThreadNew(PrioInversionTask1, NULL, &prioTask1_attr);
	prioTask2 = osThreadNew(PrioInversionTask2, NULL, &prioTask2_attr);
	prioTask1 = osThreadNew(PrioInversionTask3, NULL, &prioTask3_attr);

#ifdef USE_MUTEX
	myMutex = osMutexNew(&myMutex_attr);
#endif
#ifndef USE_MUTEX
	mySemaphore = osSemaphoreNew(1, 0, NULL);
#endif
	// Exit initialization
	osThreadExit();
}

void PrioInversionTask1(void *argument) {
	for(;;) {
#ifdef USE_MUTEX
		osMutexAcquire(myMutex, osWaitForever);
		osDelay(100);
		osMutexRelease(myMutex);
		osDelay(500);
#endif
#ifndef USE_MUTEX
		osSemaphoreAcquire(mySemaphore, osWaitForever);
		osDelay(100);
		osSemaphoreRelease(mySemaphore);
		osDelay(500);
#endif
	}
}

void PrioInversionTask2(void *argument) {
	for(;;) {
		osDelay(200);
	}
}

void PrioInversionTask3(void *argument) {
	for(;;) {
#ifdef USE_MUTEX
		osMutexAcquire(myMutex, osWaitForever);
		osDelay(100);
		osMutexRelease(myMutex);
		osDelay(500);
#endif
#ifndef USE_MUTEX
		osSemaphoreAcquire(mySemaphore, osWaitForever);
		osDelay(100);
		osSemaphoreRelease(mySemaphore);
		osDelay(500);
#endif
	}
}
