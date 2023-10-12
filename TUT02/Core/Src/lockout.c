/*
 * lockout.c
 *
 *  Created on: Mar 23, 2023
 *      Author: rini
 */

#include "lockout.h"

#define ANTI_LOCKOUT_DELAY
osSemaphoreId_t mySem;

void InitLockout(void *argument) {
	/* Definitions for blinkThread and UARTThread */
	osThreadId_t blinkThreadID;
	const osThreadAttr_t blinkThread_attr = {
			.name = "blinkThread",
			.stack_size = 128 * 4, /* In bytes */
			.priority = (osPriority_t) osPriorityNormal,
	};
	osThreadId_t UARTThreadID;
	const osThreadAttr_t UARTThread_attr = {
			.name = "UARTThread",
			.stack_size = 128 * 4, /* In bytes */
			.priority = (osPriority_t) osPriorityAboveNormal,
	};
	blinkThreadID = osThreadNew(StarvationTask1, NULL, &blinkThread_attr);
	UARTThreadID = osThreadNew(StarvationTask2, NULL, &UARTThread_attr);

	mySem = osSemaphoreNew(1, 0, NULL);

	// Exit initialization
	osThreadExit();
}

void LockoutTask1(void *argument) {
	for(;;) {
		if(osSemaphoreAcquire(mySem, osWaitForever) == osOK) {
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			osDelay(500);
			osSemaphoreRelease(mySem);
		}
	}
}

void LockoutTask2(void *argument) {
	for(;;) {
		if(osSemaphoreAcquire(mySem, osWaitForever) == osOK) {
			HAL_UART_Transmit(&huart2, (uint8_t*)"UARTThread\r\n", strlen("UARTThread\r\n"), HAL_MAX_DELAY);
			osSemaphoreRelease(mySem);
		}
#ifdef ANTI_LOCKOUT_DELAY
		osDelay(1000);
#endif
	}
}
