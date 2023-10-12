/*
 * myTasks.c
 *
 *  Created on: Mar 18, 2023
 *      Author: rini
 */

#include "myTasks.h"

osSemaphoreId_t semaphore;
//osSemaphoreAttr_t asdf;

// Bad practice to use a "generic" global variable
uint8_t globalVar = 1;

ringbuffer_t myBuffer = {
	.element = {0,0,0,0,0,0,0,0,0,0},
	.r_index = 0,
	.w_index = 0
};

void SemInit(uint8_t producers) {
	semaphore = osSemaphoreNew(1, 1, NULL);
	if(semaphore == NULL) {
		// Things gone wrong, should not continue
	}
	myBuffer.sem_r = osSemaphoreNew(producers, 0, NULL);
	myBuffer.sem_w = osSemaphoreNew(producers, producers, NULL);

}

bool EnterCritical() {
	osStatus_t ret = osSemaphoreAcquire(semaphore, osWaitForever);
	if(ret == osOK) {
		return true;
	} else {
		return false;
	}
}

bool LeaveCritical() {
	osStatus_t ret = osSemaphoreRelease(semaphore);
	if(ret == osOK) {
		return true;
	} else {
		return false;
	}
}

bool EnterCriticalRbWrite() {
	osStatus_t ret = osSemaphoreAcquire(myBuffer.sem_w, osWaitForever);
	if(ret == osOK) {
		return true;
	} else {
		return false;
	}
}

bool LeaveCriticalRbWrite() {
	osStatus_t ret = osSemaphoreRelease(myBuffer.sem_r);
	if(ret == osOK) {
		return true;
	} else {
		return false;
	}
}

bool EnterCriticalRbRead() {
	osStatus_t ret = osSemaphoreAcquire(myBuffer.sem_r, osWaitForever);
	if(ret == osOK) {
		return true;
	} else {
		return false;
	}
}

bool LeaveCriticalRbRead() {
	osStatus_t ret = osSemaphoreRelease(myBuffer.sem_w);
	if(ret == osOK) {
		return true;
	} else {
		return false;
	}
}

void taskOutput(char *buffer, uint8_t bufferSize) {
	HAL_UART_Transmit(&huart2,(uint8_t*)buffer, bufferSize, 10000);
}

uint8_t CriticalCalculation(uint8_t myCalcValue) {
	globalVar += myCalcValue;
	return globalVar;
}

void CriticalWrite(uint8_t myCalcValue) {
	myBuffer.element[myBuffer.w_index] += myCalcValue;
	myBuffer.w_index = (myBuffer.w_index + 1) % 9;
}

uint8_t CriticalRead() {
	uint8_t tmpValue = myBuffer.element[myBuffer.r_index];
	myBuffer.r_index = (myBuffer.r_index + 1) % 9;
	return tmpValue;
}


void Task1(void *argument) {
	uint8_t task1Value = 1;
	//uint8_t tmpValue = 0;
	char buffer[50];
	for(;;) {
		/*
		if(EnterCritical()) {
			tmpValue = CriticalCalculation(task1Value);
			LeaveCritical();
			sprintf(buffer, "%s - Val: %u\r\n", osThreadGetName(osThreadGetId()), tmpValue);
			taskOutput(buffer, strlen(buffer));
		}
		*/

		if(EnterCriticalRbWrite()) {
			CriticalWrite(task1Value);
			LeaveCriticalRbWrite();
			sprintf(buffer, "%s\r\n", osThreadGetName(osThreadGetId()));
			taskOutput(buffer, strlen(buffer));
		}
		osDelay(1000);
	}
}

void Task2(void *argument) {
	//uint8_t task2Value = 10;
	//uint8_t tmpValue = 0;
	char buffer[50];
	for(;;) {
		/*
		if(EnterCritical()) {
			tmpValue = CriticalCalculation(task2Value);
			LeaveCritical();
			sprintf(buffer, "%s - Val: %u\r\n", osThreadGetName(osThreadGetId()), tmpValue);
			taskOutput(buffer, strlen(buffer));
		}
		*/

		if(EnterCriticalRbRead()) {
			sprintf(buffer, "%s : %u\r\n", osThreadGetName(osThreadGetId()), CriticalRead());
			LeaveCriticalRbRead();
			taskOutput(buffer, strlen(buffer));
		}
		osDelay(1000);
	}
}

void ProducerTask(void *argument) {
	char buffer[50];
	osThreadId_t producerId = osThreadGetId();
	const char * producerName = osThreadGetName(producerId);
	srand(osKernelGetTickCount());
	uint8_t producerValue = rand() % 256;
	for(;;) {
		if(EnterCriticalRbWrite()) {
			CriticalWrite(producerValue);
			LeaveCriticalRbWrite();
			sprintf(buffer, "%s(%p) wrote %u\r\n", producerName, producerId, producerValue);
			taskOutput(buffer, strlen(buffer));
		}
	}
}

void ConsumerTask(void *argument) {
	char buffer[50];
	osThreadId_t consumerId = osThreadGetId();
	const char * consumerName = osThreadGetName(consumerId);
	uint8_t tmpValue = 0;
	for(;;) {
		if(EnterCriticalRbRead()) {
			tmpValue = CriticalRead();
			LeaveCriticalRbRead();
			sprintf(buffer, "%s(%p) read %u\r\n", consumerName, consumerId, tmpValue);
			taskOutput(buffer, strlen(buffer));
		}
	}
}
