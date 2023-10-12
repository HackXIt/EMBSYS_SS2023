/*
 * myTasks.c
 *
 *  Created on: Apr 16, 2023
 *      Author: rini
 */

#include "myTasks.h"

/*
 * NOTE: The FreeRTOS variant of the application is currently broken and goes into hardFault
 * I prefer CMSIS anyways and I tried making a direct clone in pure FreeRTOS API for good comparison
 * But apparently I get some kind of access violation or something, which triggers a hardFault.
 */

#ifndef USE_CMSIS

#define NO_CLEAR_ON_ENTRY	0x00
#define ULONG_MAX			0xFFFFFFFF
#define ONESHOT_TIMER		pdFALSE
#define EVENTGROUP_CLEAR_ON_EXIT	pdTRUE
#define EVENTGROUP_WAIT_ANY_BITS	pdFALSE

// Global variables
// Definition of producer names for pretty-output
// TODO Somehow I need to make this static name definition dynamic to use NUM_PRODUCERS
// But that's not really trivial or easy to do with pre-processors, since I'm not that proficient in using them
// Maybe I'll have it figured out until Assignment04, who knows...
// ALSO, shit breaks when changing NUM_PRODUCERS, since this name definition is hardcoded.
const char *producerNames[3] = {"Producer1","Producer2","Producer3"};
TaskHandle_t monitorHandle; // (Not in use currently)
EventGroupHandle_t monitorFlags; // Global variable for flag synchronization
QueueHandle_t monitorQueue; // Queue for the monitor to receive data
QueueHandle_t updateQueue; // Queue for the producers to receive data
char debugMsgBuffer[MSG_BUF_SIZE];

void StartInitializeTask(void *argument) {
	printf("Initializing application...\r\n");
	BaseType_t ret;
	//osEventFlagsAttr_t producerFlags_attr[NUM_PRODUCERS];
	TaskHandle_t *taskHandles = pvPortMalloc(sizeof(TaskHandle_t) * NUM_PRODUCERS);
	producerObj_t *producerObjects = pvPortMalloc(sizeof(producerObj_t) * NUM_PRODUCERS);
	// Initialize monitor event flags and messageQueues
	monitorFlags = xEventGroupCreate();
	if(monitorFlags == NULL) {
		// TODO handle out-of-heap (monitorFlags) during initialization
	}
	monitorQueue = xQueueCreate(NUM_PRODUCERS, sizeof(dataObj_t));
	if(monitorQueue == NULL) {
		// TODO handle out-of-heap (monitorQueue) during initialization
	}
	updateQueue = xQueueCreate(1, sizeof(dataObj_t));
	if(updateQueue == NULL) {
		// TODO handle out-of-heap (updateQueue) during initialization
	}

	// Initialize and start the producers
	for(uint8_t i = 0; i < NUM_PRODUCERS; i++) {
		//initAttributes(&producer_attributes[i], producerNames[i], (128 * 4), osPriorityLow, osThreadDetached);
		//producerFlags_attr[i].name = producerFlagNames[i];
		producerObjects[i].producerNumber = i;
		producerObjects[i].monitorQueue = &monitorQueue;
		producerObjects[i].updateQueue = &updateQueue;
		//producerObjects[i].taskFlags = osEventFlagsNew(&producerFlags_attr[i]);
		ret = xTaskCreate(ProducerTask, producerNames[i], 128, (void*)&producerObjects[i], 1, taskHandles+i);
		sprintf(debugMsgBuffer, "#%d %s", (i+1), producerNames[i]);
		verifyTaskCreation(ret, debugMsgBuffer);
	}

	// Start the monitor and provide the producer array
	ret = xTaskCreate(MonitorTask, "MonitorTask", 512, (void*)taskHandles, 2, &monitorHandle);
	sprintf(debugMsgBuffer, "#%d %s", (NUM_PRODUCERS+1), "MonitorTask");
	verifyTaskCreation(ret, debugMsgBuffer);


	vTaskDelete(NULL); // Exit initialization task
}



void producerTimerCallback (TimerHandle_t argument) {
	// T* data = (T*)pvTimerGetTimerID((TimerHandle_t)argument);
	TaskHandle_t *handle = (TaskHandle_t*)pvTimerGetTimerID(argument);
	xTaskNotify(*handle, TASK_INCREMENT, eSetBits);
}

void producerUpdate(QueueHandle_t queue, TaskHandle_t handle, uint32_t *interval) {
	dataObj_t rx_obj;
	if(xQueueReceive(queue, &rx_obj, 0) == pdTRUE) {
		if(rx_obj.taskHandle == handle) {
			*interval = rx_obj.value;
			return;
		}
		if(xQueueSendToFront(queue, (void*)&rx_obj, 0) != pdTRUE) {
			// TODO handle errQUEUE_FULL failure in producerUpdate
		}
	}
}
void producerIncrement(QueueHandle_t queue, TaskHandle_t handle, uint32_t *counter) {
	*counter += 1;
	dataObj_t tx_obj = {
		.taskHandle = handle,
		.value = *counter
	};
	if(xQueueSend(queue, (void*)&tx_obj, 0) == pdTRUE) {
		xEventGroupSetBits(monitorFlags, QUEUE_UPDATE);
	} else {
		// TODO handle errQUEUE_FULL failure in producerIncrement
	}
}

void ProducerTask(void *argument) {
	// Parse flags argument
	producerObj_t producerObj = *((producerObj_t*)argument);
	TaskHandle_t producerHandle = xTaskGetCurrentTaskHandle();
	QueueHandle_t monitorQueue = *(producerObj.monitorQueue);
	QueueHandle_t updateQueue = *(producerObj.updateQueue);
	uint32_t counterVar = 0;
	TickType_t counterInterval = STARTING_INTERVAL;
	TimerHandle_t producerTimer = xTimerCreate("ProducerTimer", counterInterval, ONESHOT_TIMER, (void *)&producerHandle, producerTimerCallback);
	xTimerStart(producerTimer, counterInterval); // Shoots once
	BaseType_t ret;
	uint32_t flag = 0;
	for(;;) {
		ret = xTaskNotifyWait(NO_CLEAR_ON_ENTRY, ULONG_MAX, &flag, portMAX_DELAY);
		if(ret != pdFALSE) {
			// TODO handle unexpected timeout from notify?
		}
		switch(flag) {
		case TASK_UPDATE:
			producerUpdate(updateQueue, producerHandle, &counterInterval);
			break;
		case TASK_INCREMENT:
			producerIncrement(monitorQueue, producerHandle, &counterVar);
			break;
		}
		// Restart timer
		xTimerStart(producerTimer, counterInterval);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart == &huart2) {
		xEventGroupSetBits(monitorFlags, RX_BYTE);
	}
}

void MonitorTask(void *argument) {
	// Parse producer argument
	TaskHandle_t *producers = (TaskHandle_t *)argument;
	uint8_t buffer_index = 0;
	char * rx_buffer = pvPortMalloc(sizeof(char) * buffer_index+1);
	rx_buffer[0] = '\0';
	char * buffer_tmp = NULL;
	bool receiving_input = false;
	BaseType_t ret;
	taskInfoObj_t taskInfo;
	dataObj_t rx_obj;
	EventBits_t flag = 0;
	HAL_UART_Receive_IT(&huart2, (uint8_t*)rx_buffer, 1);
	for(;;) {
		flag = xEventGroupWaitBits(monitorFlags, RX_BYTE | INPUT | QUEUE_UPDATE, EVENTGROUP_CLEAR_ON_EXIT, EVENTGROUP_WAIT_ANY_BITS, portMAX_DELAY);
		switch(flag) {
		case RX_BYTE:
			if(!receiving_input) { // Block monitor output for duration of CMD-Input
				receiving_input = true;
				sprintf(debugMsgBuffer, "Output halted - CMD: ");
				HAL_UART_Transmit(&huart2, (uint8_t*)debugMsgBuffer, strlen(debugMsgBuffer), UPDATE_TIMEOUT);
			}
			if(rx_buffer[buffer_index] == '\r') {
				rx_buffer[buffer_index] = '\0'; // NUL-terminate current string buffer
				xEventGroupSetBits(monitorFlags, INPUT); // Trigger input parsing on carriage return
				buffer_index = 0;
				HAL_UART_Receive_IT(&huart2, (uint8_t*)rx_buffer, 1);
				break;
			} else {
				HAL_UART_Transmit(&huart2, (uint8_t*)rx_buffer+buffer_index, 1, UPDATE_TIMEOUT); // Output input character
			}
			// Prepare & Resize buffer for next character
			// buffer_index+1 == Size of buffer (Reminder: index starts at 0)
			// buffer_index+2 == Size of buffer + 1 new element
			buffer_tmp = pvPortMalloc(sizeof(char) * (buffer_index+2));
			if(buffer_tmp == NULL) {
				// TODO Handle out of memory exception
			} else {
				memcpy(buffer_tmp, rx_buffer, (buffer_index+1)); // Copy existing data
				vPortFree(rx_buffer); // Free old string memory
				rx_buffer = buffer_tmp; // Re-assign string memory to new block
				buffer_tmp = NULL; // Re-assign temporary buffer to NULL;
				buffer_index++; // Increase buffer index to next character position
				HAL_UART_Receive_IT(&huart2, (uint8_t*)rx_buffer+buffer_index, 1);
			}
			break;
		case INPUT:
			printf("\r\n");
			receiving_input = false;
			parseStatus_t parse_ret = parseInput(rx_buffer, &taskInfo);
			switch(parse_ret) {
			case parseOK:
				if(taskInfo.taskNumber >= NUM_PRODUCERS) {
					printf("Task index out of bounds!\r\n");
				}
				dataObj_t tx_obj = {
						.taskHandle = producers[taskInfo.taskNumber],
						.value = taskInfo.targetInterval
				};
				printf("Updating %s interval to %lums.\r\n", pcTaskGetName(tx_obj.taskHandle), (unsigned long)tx_obj.value);
				if(xQueueSend(updateQueue, &tx_obj, 0) != pdTRUE) {
					// TODO handle errQUEUE_FULL failure in parseOK case
				}
				xTaskNotify(producers[taskInfo.taskNumber], TASK_UPDATE, eSetBits);
				break;
			case parseException:
				printf("Input parsing exception.\r\n");
				break;
			case parseFailure:
				printf("Input parsing error.\r\n");
				break;
			}
			break;
		case QUEUE_UPDATE:
			ret = xQueueSend(monitorQueue, &rx_obj, 0);
			if(receiving_input) break;
			if(ret == osOK) {
				printf("%s: %lu\r\n", pcTaskGetName(rx_obj.taskHandle), (unsigned long)rx_obj.value);
			} else {
				printf("Monitor queue error.\r\n");
			}
			break;
		}
	}
}

#endif
