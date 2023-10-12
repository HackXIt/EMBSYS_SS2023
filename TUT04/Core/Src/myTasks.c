/*
 * myTasks.c
 *
 *  Created on: Apr 17, 2023
 *      Author: rini
 */

#include "myTasks.h"

uint8_t rx_byte;
uint8_t buffer_index = 0;
char * rx_buffer;
char * tmp_buffer;
bool stringReady;

void InitTasks(void *argument) {
	osThreadAttr_t monitorTask_attributes = {
			.name = "MonitorTask",
			.stack_size = 512 * 4,
			.priority = osPriorityNormal
	};
	osThreadNew(MonitorTask, NULL, &monitorTask_attributes);
	osThreadExit();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart == &huart2) {
		rx_buffer[buffer_index] = rx_byte;
		HAL_UART_Transmit(&huart2, &rx_byte, 1, 10000);
		if((char)rx_byte == '\n') {
			// String is done
			stringReady = true;
		}
		buffer_index++;
		// Check buffer index size => increase array of char
		// realloc des buffer => neue größe
		//rx_buffer = realloc(rx_buffer, buffer_index);
		/* NICHT direkt in diesem Callback machen, sondern woanders
		tmp_buffer = pvPortMalloc(sizeof(char) * (buffer_index+2));
		strcpy(tmp_buffer, rx_buffer);
		tmp_buffer[buffer_index+1] = '\0';
		vPortFree(rx_buffer);
		rx_buffer = tmp_buffer;
		HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
		*/
	}
}
/*
void producerCallback(void *arg) {
	osSemaphoreId_t *prod_sem = (osSemaphoreId_t*) arg;
	osSemaphoreRelease(prod_sem);
}

void ProducerTask(void *argument) {
	osSemaphoreId_t updateSemaphore = osSemaphoreNew(1, 0, NULL);
	osTimerId_t producerTimer = osTimerNew(producerCallback, osTimerOnce, &updateSemaphore);
	osTimerStart(producerTimer, 1000U);
}
*/

void MonitorTask(void *argument) {
	/*
	int targetValue;
	int targetTask;
	char *welcomeMsg = "hello world\r\n";
	rx_buffer = pvPortMalloc(sizeof(char) * 2);
	rx_buffer[0] = '\0';
	rx_buffer[1] = '\0';
	HAL_UART_Transmit(&huart2, (uint8_t*)welcomeMsg, strlen(welcomeMsg), 10000);
	HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
	*/
	QueueHandle_t myQueue = xQueueCreate(5, sizeof(uint8_t));
	vQueueAddToRegistry(myQueue, "myQueue");
	uint8_t val0 = 0;
	xQueueSend(myQueue, (void*)&val0, 1000);
	uint8_t val1 = 1;
	xQueueSend(myQueue, (void*)&val1, 1000);
	uint8_t val2 = 2;
	xQueueSend(myQueue, (void*)&val2, 1000);
	for(;;) {
		//HAL_UART_Receive(&huart2, &rx_byte, 1, 100000);
		/*
		if(stringReady) {
			// Input parsen
			parseInput(rx_buffer, buffer_index, &targetTask, &targetValue);
		}
		*/
		// Ausgabe von monitor task tätigkeiten

	}
}
