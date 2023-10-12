/*
 * myTasks.c
 *
 *  Created on: Mar 30, 2023
 *      Author: RINI
 */
#include "myTasks.h"

char *shared_string;
char *dest_string;

osSemaphoreId_t read_x;
osSemaphoreId_t write_x;

void InitTasks(void *argument) {
	osThreadAttr_t receiver_attributes = {
			.name = "receiverTask",
			.priority = osPriorityLow,
	};
	osThreadAttr_t sender_attributes = {
			.name = "senderTask",
			.priority = osPriorityLow,
	};
	osThreadAttr_t monitor_attributes = {
			.name = "monitorTask",
			.priority = osPriorityLow,
	};

	read_x = osSemaphoreNew(1, 0, NULL);
	write_x = osSemaphoreNew(1, 0, NULL);

	osThreadNew(ReceiverTask, NULL, &receiver_attributes);
	osThreadNew(SenderTask, NULL, &sender_attributes);
	osThreadNew(MonitorTask, NULL, &monitor_attributes);
	osThreadExit();
}

void ReceiverTask(void *argument) {
	char rx_c = '0';
	uint8_t rx_index = 0;
	char rx_buffer[10];
	HAL_StatusTypeDef ret;
	for(;;) {
		ret = HAL_UART_Receive(&huart2, (uint8_t*)&rx_c, 1, HAL_MAX_DELAY);
		if(rx_c == '\r') {
			rx_buffer[rx_index] = rx_c;
			rx_buffer[rx_index+1] = '\n';
			rx_buffer[rx_index+2] = '\0';
			shared_string = pvPortMalloc(sizeof(char) * strlen(rx_buffer)+1);
			if(shared_string != NULL) {
				strcpy(shared_string, rx_buffer);
				/*
				uint8_t size_x = sizeof(*shared_string) / sizeof(shared_string[0]) - 1;
				memset(shared_string[size_x], '\0', 1);
				*/
			}
			rx_index = 0;
			memset(rx_buffer, '\0', sizeof(rx_buffer));
			osSemaphoreRelease(read_x);
		} else if(ret == HAL_OK) {
			rx_buffer[rx_index] = rx_c;
			rx_index++;
			if(rx_index >= 8) {
				rx_index = 0;
			}
		}
	}
}

/*
void SenderTask(void *argument) {
	char *buffer;
	uint8_t counter = 0;
	osStatus_t ret;
	for(;;) {
		ret = osSemaphoreAcquire(write_x, osWaitForever);
		if(ret == osOK) {
			HAL_UART_Transmit(&huart2, (uint8_t*)dest_string, strlen(dest_string), 10000);
			vPortFree(dest_string);
		}
	}
}
*/

void MonitorTask(void *argument) {
	uint8_t counter = 0;
	osStatus_t ret;
	for(;;) {
		ret = osSemaphoreAcquire(read_x, osWaitForever);
		if(ret == osOK) {
			/*
			dest_string = pvPortMalloc(sizeof(char) * strlen(shared_string)+1);
			if(dest_string != NULL) {
				strcpy(dest_string, shared_string);
				uint8_t size_x = sizeof(*dest_string) / sizeof(dest_string[0]) - 1;
				memset(dest_string[size_x], '\0', 1);
			}
			vPortFree(shared_string);
			*/
			uint8_t check_return;
			check_return = check_string(shared_string);
			//osSemaphoreRelease(write_x);
			// Output of received producer stuff
		}
	}
}

void ProducerTask(void *argument) {
	uint8_t myDelay;
	uint8_t myCounter;
	for(;;) {
		// Do some stuff with counters and queues
	}

}
