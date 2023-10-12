/*
 * myTasks.c
 *
 *  Created on: Apr 22, 2023
 *      Author: rini
 */

#include "myTasks.h"

static bool received_byte = false;

#ifdef USE_CMSIS

void StartInitializeTask(void *argument) {
	printf("Started task initialization...\r\n");
	osThreadAttr_t producer1_attr = {
			.name = "Producer1",
			.priority = osPriorityLow
	};
	osThreadAttr_t producer2_attr = {
			.name = "Producer2",
			.priority = osPriorityLow
	};
	osThreadAttr_t producer3_attr = {
			.name = "Producer3",
			.priority = osPriorityLow
	};
	osThreadAttr_t *producer_attributes[3];
	producer_attributes[0] = &producer1_attr;
	producer_attributes[1] = &producer2_attr;
	producer_attributes[2] = &producer3_attr;
	//osThreadId_t producerHandles[3];
	monArg_t monitorArgument = {
		.producerHandles = {NULL},
		.monitorQueue = NULL,
		.producerQueue = NULL
	};
	prodArg_t producerArguments[3];

	// Queue for the monitor to receive data
	monitorArgument.monitorQueue = osMessageQueueNew(3, sizeof(queueItem_t), NULL);
	// Queue for the producers to receive data
	monitorArgument.producerQueue = osMessageQueueNew(1, sizeof(parsedObj_t), NULL);
	if(monitorArgument.monitorQueue == NULL || monitorArgument.producerQueue == NULL) {
			printf("Failed to create queues.\r\n");
	}

	for(uint8_t i = 0; i < 3; i++) {
		producerArguments[i].number = i;
		producerArguments[i].monitorQueue = monitorArgument.monitorQueue;
		producerArguments[i].producerQueue = monitorArgument.producerQueue;
		monitorArgument.producerHandles[i] = osThreadNew(ProducerTask, (void*)&producerArguments[i], producer_attributes[i]);
		if(monitorArgument.producerHandles[i] == NULL) {
			printf("Failed to create producer task #%d.\r\n", i);
		} else {
			printf("Created producer task #%d.\r\n", i);
		}
	}

	osThreadAttr_t monitor_attributes = {
			.name = "MonitorTask",
			.stack_size = 512 * 4,
			.priority = osPriorityNormal
	};
	osThreadId_t monitorHandle = osThreadNew(MonitorTask, (void*)&monitorArgument, &monitor_attributes);
	if(monitorHandle == NULL) {
		printf("Failed to create monitor task.\r\n");
	} else {
		printf("Created monitor task.\r\n");
	}
	osThreadExit(); // Exit initialization task
}

void ProducerTask(void *argument) {
	prodArg_t producer = *((prodArg_t*)argument);
	// producer.monitorQueue => SENDET DATENOBJEKTE AN MONITOR
	// producer.producerQueue => EMPFÄNGT DATENOBJEKTE VON MONITOR
	uint32_t counterInterval = 1000;
	osStatus_t ret;
	parsedObj_t rx_parsed_obj;
	queueItem_t tx_queue_obj = {
			.taskNum = producer.number,
			.value = 0
	};
	for(;;) {
		ret = osMessageQueueGet(producer.producerQueue, &rx_parsed_obj, 0, 0);
		switch(ret) {
		case osOK:
			if(rx_parsed_obj.taskNumber == producer.number) {
				counterInterval = rx_parsed_obj.targetInterval;
			} else {
				osMessageQueuePut(producer.producerQueue, &rx_parsed_obj, 0, 0);
				// BEWARE OF MISSING ERROR HANDLING
			}
			break;
		case osErrorTimeout:
			// This should not occur, since we don't use timeouts => undefined behaviour
			break;
		case osErrorResource:
			// EXPECTED behavior when queue is empty
			break;
		case osErrorParameter:
			// Actual problematic ERROR occurred => panic!
			break;
		default:
			// Unhandled error occoured => panic!
			break;
		}
		ret = osMessageQueuePut(producer.monitorQueue, &tx_queue_obj, 0, 0);
		switch(ret) {
		case osOK:
			// IT WORKED => increment counter value
			tx_queue_obj.value++;
			break;
		case osErrorTimeout:
			// This should not occur, since we don't use timeouts => undefined behaviour
			break;
		case osErrorResource:
			// PROBLEM behavior => queue is full => monitor is possibly dead!
			break;
		case osErrorParameter:
			// Actual problematic ERROR occurred => panic!
			break;
		default:
			// Unhandled error occoured => panic!
			break;
		}
		osDelay(counterInterval);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart == &huart2) {
		received_byte = true;
	}
}

void MonitorTask(void *argument) {
	monArg_t monitor = *((monArg_t*)argument);
	// monitor.monitorQueue => EMPFÄNGT DATENOBJEKTE VON PRODUCERN
	// monitor.producerQueue => SENDET DATENOBJEKTE AN PRODUCER
	char rx_uart_buffer[128];
	char *input = NULL;
	uint8_t write_index = 0;
	char tmp = '\0';
	bool input_ready = false;
	bool output_ready = false;
	bool receiving = false;
	parsedObj_t tx_parsed_obj;
	queueItem_t rx_queue_obj;
	osStatus_t ret;
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&tmp, 1);
	for(;;) {
		ret = osMessageQueueGet(monitor.monitorQueue, &rx_queue_obj, 0, 100); // Poll queue roughly every ~100 ticks
		if(ret == osOK) {
			output_ready = true;
		} else if(ret != osErrorTimeout) {
			// If the error is anything other than a failure to get from the queue, report it
			printf("Error handling monitor queue: %d\r\n", ret);
			osDelay(5000);
		}
		if(output_ready) {
			// Do not output to UART when receiving cmd input for usability
			if(!receiving) {
				printf("Task %d: %lu\r\n", rx_queue_obj.taskNum, rx_queue_obj.value);
			}
			output_ready = false;
		}
		if(received_byte) {
			receiving = true;
			HAL_UART_Transmit(&huart2, (uint8_t*)&tmp, 1, 1000);
			if(tmp == '\r') {
				rx_uart_buffer[write_index] = '\0';
				// DYNAMIC_INPUT_ALLOCATION START
				input = pvPortMalloc(sizeof(char) * (write_index+1));
				if(input == NULL) {
					printf("Memory error!\r\n");
				} else {
					memcpy(input, rx_uart_buffer, (write_index+1));
					input_ready = parseInput(input, &tx_parsed_obj);
				}
				vPortFree(input);
				// DYNAMIC_INPUT_ALLOCATION END
				/* alternative to DYNAMIC_INPUT_ALLOCATION:
				 * input_ready = parseInput(rx_uart_buffer, &tx_parsed_obj);
				 */
				write_index = 0;
				receiving = false;
			} else {
				rx_uart_buffer[write_index] = tmp;
				write_index++;
				if(write_index >= 128) {
					printf("Input limit reached!\r\n");
					write_index = 0;
				}
			}
			received_byte = false;
			HAL_UART_Receive_IT(&huart2, (uint8_t*)&tmp, 1);
		}
		if(input_ready) {
			ret = osMessageQueuePut(monitor.producerQueue, &tx_parsed_obj, 0, 0);
			if(ret != osOK) {
				printf("Failed to put data in producerQueue: %d\r\n", ret);
			}
			input_ready = false;
		}
	}
}

#endif

#ifndef USE_CMSIS

void StartInitializeTask(void *argument) {

}

void ProducerTask(void *argument) {

}

void MonitorTask(void *argument) {

}

#endif
