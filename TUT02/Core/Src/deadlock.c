/*
 * deadlock.c
 *
 *  Created on: Mar 23, 2023
 *      Author: rini
 */

#include "deadlock.h"

SemaphoreHandle_t xSem;
SemaphoreHandle_t ySem;

void InitDeadlock(void *argument) {
	xSem = xSemaphoreCreateMutex();
	ySem = xSemaphoreCreateMutex();

	xTaskCreate(DeadlockTask1, "Deadlock1", 128, NULL, 1, NULL);
	xSemaphoreGive(xSem);
	xSemaphoreGive(ySem);
}

void DeadlockTask1(void *argument) {
	BaseType_t ret;
	ret = xSemaphoreTake(xSem, portMAX_DELAY);
	char buffer[] = "Task 1 runs...\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 10000);
	xTaskCreate(DeadlockTask2, "Deadlock2", 128, NULL, 2, NULL);
	ret = xSemaphoreTake(ySem, portMAX_DELAY); // DEADLOCK
	ret = xSemaphoreGive(xSem);
	sprintf(buffer, "Task 1 ends...\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 10000);
	ret = xSemaphoreGive(ySem);
	vTaskDelete(NULL);
}

void DeadlockTask2(void *argument) {
	ret = xSemaphoreTake(ySem, portMAX_DELAY);
	char buffer[] = "Task 2 runs...\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 10000);
	ret = xSemaphoreTake(xSem, portMAX_DELAY); // DEADLOCK
	ret = xSemaphoreGive(ySem);
	sprintf(buffer, "Task 1 ends...\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 10000);
	ret = xSemaphoreGive(xSem);
	vTaskDelete(NULL);
}
