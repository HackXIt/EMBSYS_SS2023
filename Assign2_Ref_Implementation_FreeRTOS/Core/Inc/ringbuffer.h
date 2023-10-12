/*
 * ringbuffer.h
 *
 *  Created on: Mar 20, 2023
 *      Author: rini
 */

#ifndef INC_RINGBUFFER_H_
#define INC_RINGBUFFER_H_

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define RINGBUFFER_SIZE			10

typedef struct ringbuffer_element {
	TaskHandle_t prod_id;
	uint8_t prod_value;
}rb_elem_t;

typedef struct ringbuffer {
	rb_elem_t elements[RINGBUFFER_SIZE];
	SemaphoreHandle_t sem_read;
	SemaphoreHandle_t sem_write;
	uint8_t r_index;
	uint8_t w_index;
}ringbuffer_t;

bool Ringbuffer_Put(rb_elem_t elem);
bool Ringbuffer_Get(rb_elem_t *elem);

void Ringbuffer_Init();

#endif /* INC_RINGBUFFER_H_ */
