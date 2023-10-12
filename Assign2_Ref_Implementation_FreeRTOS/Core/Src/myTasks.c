/*
 * myTasks.c
 *
 *  Created on: Mar 20, 2023
 *      Author: rini
 */

#include "myTasks.h"

void StartTasks(void *argument) {
	Ringbuffer_Init();
	for(uint8_t i = 0; i < NUM_PRODS; i++) {
		//osDelay(2000);
		xTaskCreate(ProducerTask, "producerTask", 64, NULL, 1, NULL);
	}
	vTaskDelete(NULL);
}

#define NO_RETARGET

void ConsumerTask(void *argument) {
	rb_elem_t tmpElement = {
			.prod_id = NULL,
			.prod_value = 0
	};
	for(;;) {
		if(Ringbuffer_Get(&tmpElement)) {
#ifndef NO_RETARGET
			printf("Producer %p => %u\r\n", tmpElement.prod_id, tmpElement.prod_value);
#endif
#ifdef NO_RETARGET
			char buffer[50];
			sprintf(buffer, "Producer %p => %u\r\n", tmpElement.prod_id, tmpElement.prod_value);
			HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 10000);
#endif
		}
		//vTaskDelay(2000);
		//osDelay(2000);
		for(uint32_t i = 0; i < 2000; i++) {

		}
	}
}

void ProducerTask(void *argument) {
	srand(xTaskGetTickCount());
	rb_elem_t tmpElement = {
			.prod_id = xTaskGetCurrentTaskHandle(),
			.prod_value = rand() % 0xFF
	};
	for(;;) {
		if(Ringbuffer_Put(tmpElement)) {
			vTaskDelete(NULL);
		}
	}
}
