/*
 * myTasks.c
 *
 *  Created on: Apr 16, 2023
 *      Author: rini
 */

#include "myTasks.h"

#ifdef USE_CMSIS

// Global variables
static osMessageQueueAttr_t monitorQueue_attr = {
		.name = "MonitorQueue"
};
static osMessageQueueAttr_t updateQueue_attr = {
		.name = "updateQueue"
};
static osEventFlagsAttr_t monitorFlags_attr = {
	.name = "MonitorFlags"
};
// Definition of producer names for pretty-output
// TODO Somehow I need to make this static name definition dynamic to use NUM_PRODUCERS
// But that's not really trivial or easy to do with pre-processors, since I'm not that proficient in using them
// Maybe I'll have it figured out until Assignment04, who knows...
// ALSO, shit breaks when changing NUM_PRODUCERS, since this name definition is hardcoded.
const char *producerNames[3] = {"Producer1","Producer2","Producer3"};
osThreadId_t monitorHandle; // (Not in use currently)
osEventFlagsId_t monitorFlags; // Global variable for flag synchronization
osMessageQueueId_t monitorQueue; // Queue for the monitor to receive data
osMessageQueueId_t updateQueue; // Queue for the producers to receive data
char debugMsgBuffer[MSG_BUF_SIZE];

void StartInitializeTask(void *argument) {
	printf("Initializing application...\r\n");
	//char *producerFlagNames[NUM_PRODUCERS] = {"ProdFlags1", "ProdFlags2", "ProdFlags3"};
	osThreadAttr_t producer_attributes[NUM_PRODUCERS];
	//osEventFlagsAttr_t producerFlags_attr[NUM_PRODUCERS];
	osThreadId_t *taskHandles = pvPortMalloc(sizeof(osThreadId_t) * NUM_PRODUCERS);
	producerObj_t *producerObjects = pvPortMalloc(sizeof(producerObj_t) * NUM_PRODUCERS);
	// Initialize monitor event flags and messageQueues
	monitorFlags = osEventFlagsNew(&monitorFlags_attr);
	monitorQueue = osMessageQueueNew(NUM_PRODUCERS, sizeof(dataObj_t), &monitorQueue_attr);
	updateQueue = osMessageQueueNew(1, sizeof(dataObj_t), &updateQueue_attr);

	// Initialize and start the producers
	for(uint8_t i = 0; i < NUM_PRODUCERS; i++) {
		initAttributes(&producer_attributes[i], producerNames[i], (128 * 4), osPriorityLow, osThreadDetached);
		//producerFlags_attr[i].name = producerFlagNames[i];
		producerObjects[i].producerNumber = i;
		producerObjects[i].monitorQueue = &monitorQueue;
		producerObjects[i].updateQueue = &updateQueue;
		//producerObjects[i].taskFlags = osEventFlagsNew(&producerFlags_attr[i]);
		taskHandles[i] = osThreadNew(ProducerTask, &producerObjects[i], &producer_attributes[i]);
		sprintf(debugMsgBuffer, "#%d %s", (i+1), producerNames[i]);
		verifyTaskCreation(taskHandles[i], debugMsgBuffer);
	}

	// Start the monitor and provide the producer array
	osThreadAttr_t monitor_attributes = {
			.name = "MonitorTask",
			.stack_size = 512 * 4,
			.priority = osPriorityNormal
	};
	monitorHandle = osThreadNew(MonitorTask, taskHandles, &monitor_attributes);
	sprintf(debugMsgBuffer, "#%d %s", (NUM_PRODUCERS+1), monitor_attributes.name);
	verifyTaskCreation(monitorHandle, debugMsgBuffer);

	osThreadExit(); // Exit initialization task
}



static void producerTimerCallback (void *argument) {
	osThreadId_t arg = *((osThreadId_t*)argument);
	osThreadFlagsSet(arg, TASK_INCREMENT);
}

void producerUpdate(osMessageQueueId_t queue, osThreadId_t handle, uint32_t *interval) {
	dataObj_t rx_obj;
	if(handleOSStatus(osMessageQueueGet(queue, &rx_obj, NULL, UPDATE_TIMEOUT))) {
		if(rx_obj.taskHandle == handle) {
			*interval = rx_obj.value;
			return;
		}
		if(handleOSStatus(osMessageQueuePut(queue, &rx_obj, 1, UPDATE_TIMEOUT))) {
			// object was put back at end of queue, maybe notify?
			// Not sure what else to do here
		}
	}
}
void producerIncrement(osMessageQueueId_t queue, osThreadId_t handle, uint32_t *counter) {
	*counter += 1;
	dataObj_t tx_obj = {
		.taskHandle = handle,
		.value = *counter
	};
	if(handleOSStatus(osMessageQueuePut(queue, &tx_obj, 1, osWaitForever))) {
		handleFlagReturn(osEventFlagsSet(monitorFlags, QUEUE_UPDATE));
	} else {
		// TODO handle queue error on producerIncrement
	}
}

void ProducerTask(void *argument) {
	// Parse flags argument
	producerObj_t producerObj = *((producerObj_t*)argument);
	osThreadId_t producerHandle = osThreadGetId();
	osMessageQueueId_t monitorQueue = *(producerObj.monitorQueue);
	osMessageQueueId_t updateQueue = *(producerObj.updateQueue);
	uint32_t counterVar = 0;
	uint32_t counterInterval = STARTING_INTERVAL;
	//osSemaphoreId_t updateSemaphore = osSemaphoreNew(1, 0, NULL);
	osTimerId_t producerTimer = osTimerNew(producerTimerCallback, osTimerOnce, &producerHandle, NULL);
	osTimerStart(producerTimer, counterInterval); // Shoots once
	//osStatus_t ret;
	uint32_t flag = 0;
	for(;;) {
		flag = handleFlagReturn(osThreadFlagsWait(TASK_UPDATE | TASK_INCREMENT, osFlagsWaitAny, osWaitForever));
		switch(flag) {
		case TASK_UPDATE:
			producerUpdate(updateQueue, producerHandle, &counterInterval);
			break;
		case TASK_INCREMENT:
			producerIncrement(monitorQueue, producerHandle, &counterVar);
			break;
		}
		// Restart timer
		osTimerStart(producerTimer, counterInterval);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart == &huart2) {
		osEventFlagsSet(monitorFlags, RX_BYTE);
	}
}

void MonitorTask(void *argument) {
	// Parse producer argument
	osThreadId_t *producers = (osThreadId_t *)argument;
	uint8_t buffer_index = 0;
	char * rx_buffer = pvPortMalloc(sizeof(char) * buffer_index+1);
	rx_buffer[0] = '\0';
	char * buffer_tmp = NULL;
	bool receiving_input = false;
	osStatus_t ret;
	taskInfoObj_t taskInfo;
	dataObj_t rx_obj;
	uint32_t flag = 0;
	HAL_UART_Receive_IT(&huart2, (uint8_t*)rx_buffer, 1);
	for(;;) {
		flag = handleFlagReturn(osEventFlagsWait(monitorFlags, RX_BYTE | INPUT | QUEUE_UPDATE, osFlagsWaitAny, osWaitForever));
		switch(flag) {
		case RX_BYTE:
			// 'T'
			// buffer_index == 1
			if(!receiving_input) { // Block monitor output for duration of CMD-Input
				receiving_input = true;
				sprintf(debugMsgBuffer, "Output halted - CMD: ");
				HAL_UART_Transmit(&huart2, (uint8_t*)debugMsgBuffer, strlen(debugMsgBuffer), UPDATE_TIMEOUT);
			}
			if(rx_buffer[buffer_index] == '\r') {
				rx_buffer[buffer_index] = '\0'; // NUL-terminate current string buffer
				osEventFlagsSet(monitorFlags, INPUT); // Trigger input parsing on carriage return
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
					break;
				}
				dataObj_t tx_obj = {
						.taskHandle = producers[taskInfo.taskNumber],
						.value = taskInfo.targetInterval
				};
				printf("Updating %s interval to %lums.\r\n", osThreadGetName(tx_obj.taskHandle), (unsigned long)tx_obj.value);
				handleOSStatus(osMessageQueuePut(updateQueue, &tx_obj, 1, osWaitForever));
				handleFlagReturn(osThreadFlagsSet(producers[taskInfo.taskNumber], TASK_UPDATE));
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
			ret = osMessageQueueGet(monitorQueue, &rx_obj, NULL, osWaitForever);
			if(receiving_input) break;
			if(ret == osOK) {
				printf("%s: %lu\r\n", osThreadGetName(rx_obj.taskHandle), (unsigned long)rx_obj.value);
			} else {
				printf("Monitor queue error.\r\n");
			}
			break;
		}
	}
}

#endif
