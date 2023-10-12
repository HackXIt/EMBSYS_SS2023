/*
 * starvation.c
 *
 *  Created on: Mar 23, 2023
 *      Author: rini
 */

#include "starvation.h"

#define ANTI_STARVATION_DELAY

void InitStarvation(void *argument) {
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

	// Exit initialization
	osThreadExit();
}

void StarvationTask1(void *argument) {
	for(;;) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osDelay(500);
	}
}

void StarvationTask2(void *argument) {
	for(;;) {
		HAL_UART_Transmit(&huart2, (uint8_t*)"UARTThread\r\n", strlen("UARTThread\r\n"), HAL_MAX_DELAY);
#ifdef ANTI_STARVATION_DELAY
		osDelay(1000);
#endif
	}
}
