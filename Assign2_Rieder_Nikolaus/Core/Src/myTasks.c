/*
 * myTasks.c
 *
 *  Created on: Mar 16, 2023
 *      Author: rini
 */

#include "myTasks.h"

//#define NO_RETARGET

// Static array of possible producer names
static char *producerNames[10] = {
		"producerTask0",
		"producerTask1",
		"producerTask2",
		"producerTask3",
		"producerTask4",
		"producerTask5",
		"producerTask6",
		"producerTask7",
		"producerTask8",
		"producerTask9"
};

void StartTasks(void *argument) {
	uint8_t *producerCount = (uint8_t*)argument;
	//char taskName[14];
	osThreadAttr_t *producer_attributes = (osThreadAttr_t *) pvPortMalloc(sizeof(osThreadAttr_t) * *producerCount);
	/*
	 * This is what one of those attributes should look like:
	osThreadAttr_t producer_attributes = {
	  .name = taskName,
	  .stack_size = 64 * 4,
	  .priority = (osPriority_t) osPriorityLow,
	};
	*/
	for(uint8_t i = 0; i < *producerCount; i++) {
		producer_attributes[i].name = producerNames[i];
		producer_attributes[i].stack_size = 64 * 4;
		producer_attributes[i].priority = (osPriority_t) osPriorityLow;
		osThreadNew(ProducerTask, NULL, &producer_attributes[i]);
	}
	osThreadExit();
}

void ProducerTask(void *argument) {
	srand(osKernelGetSysTimerCount());
	rb_elem_t producedElement = {
			.producer_id = osThreadGetId(),
			.producer_value = (rand() % 0xFF)
	};
	for(;;) {
		if(!Ringbuffer_Put(producedElement)) {
			osDelay(1000);
		} else {
			// Calculate next round
			producedElement.producer_value = rand() % 0xFF;
		}
	}
}
void ConsumerTask(void *argument) {
	rb_elem_t consumedElement = {
			.producer_id = NULL,
			.producer_value = 0
	};
#ifdef NO_RETARGET
	char buffer[255];
#endif
	for(;;) {
		if(!Ringbuffer_Get(&consumedElement)) {
			// Yield execution to another READY thread with same priority;
			// (Doesn't work since it doesn't set current thread to BLOCKED)
			//osThreadYield();
			osDelay(1000); // Hand over execution to another thread by blocking this thread
		} else {
#ifndef NO_RETARGET
			const char * taskName = osThreadGetName(consumedElement.producer_id);
			if(taskName == NULL) {
				printf("NULL(%p) => %u\r\n",
									consumedElement.producer_id,
									consumedElement.producer_value);
			} else {
				printf("%s(%p) => %u\r\n",
									taskName,
									consumedElement.producer_id,
									consumedElement.producer_value);
			}
#endif
#ifdef NO_RETARGET
			sprintf(buffer, "%s(%p) => %u\r\n",
					osThreadGetName(consumedElement.producer_id),
					consumedElement.producer_id,
					consumedElement.producer_value);
			HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 10000);
#endif
		}
	}
}
