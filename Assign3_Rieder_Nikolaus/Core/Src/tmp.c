/*
 * tmp.c
 *
 *  Created on: Apr 21, 2023
 *      Author: rini
 */

#include "usart.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include <stdbool.h>


bool other_action;
bool next_interrupt;
bool parse_input;


typedef struct dataObj {
	bool other_action;
	bool next_interrupt;
}dataObj_t;

/*
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	if(huart == &huart2) {
		// Do something at half
		receiving = true;
		memcpy(tmp_buffer, msgBuffer, 3);
		write_index += 3;
	}
}
*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart == &huart2) {
		// Do something at complete
		next_interrupt = true;

	}
}

void initTask(void *argument) {
	osMessageQueueId_t theQueue = osMessageQueueNew(1, sizeof(dataObj_t), NULL);

	osThreadNew(otherTask, &theQueue, NULL);
	// initialize all tasks

	osThreadExit();
}
osStatus_t ret = osMessageQueueGet(theQueue, argument, 0, osWaitForever);


void otherTask(void *argument) {
	osMessageQueueId_t theQueue = *((osMessageQueueId_t*)argument);
	osMessageQueuePut(theQueue, msg_ptr, msg_prio, osWaitForever);
}

void otherTask2(void *argument) {
	osMessageQueueId_t theQueue = *((osMessageQueueId_t*)argument);
	osMessageQueuePut(theQueue, msg_ptr, msg_prio, osWaitForever);
}

void myOutput() {
	vTaskSuspendAll();
	// CRITICAL SECTION
	// UNDO the suspend, don't know the function
}

void myTask(void *argument) {
	osMessageQueueId_t theQueue = *((osMessageQueueId_t*)argument);

	dataObj_t myFlags = {
			.other_action=false,
			.next_interrupt=false
	};

	char tmp_buffer[200];
	uint8_t write_index = 0;
	char msgBuffer[3];
	HAL_UART_Receive_IT(&huart2, (uint8_t*)msgBuffer, 1);
	for(;;) {
		if(parse_input) {
			// Full input received, parse it

		}
		if(next_interrupt) {
			if((write_index +1) >= 200) {

			}
			memcpy(tmp_buffer+write_index,msgBuffer, 1);
			write_index += 1;
			next_interrupt = false;
			HAL_UART_Receive_IT(&huart2, (uint8_t*)msgBuffer, 1);
		}
		if(other_action) {
			// Do the other action
			other_action = false;
			//osStatus_t ret = osMessageQueueGet(theQueue, argument, 0, 0);
			osStatus_t ret = osMessageQueueGet(theQueue, argument, 0, osWaitForever);
			handleStatusRet(ret);
			// do something with queue element
		}
	}
}
