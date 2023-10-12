/*
 * ringbuffer.c
 *
 *  Created on: Mar 20, 2023
 *      Author: rini
 */

#include "ringbuffer.h"

static ringbuffer_t ringbuffer;

bool Ringbuffer_Put(rb_elem_t elem) {
	BaseType_t ret;
	ret = xSemaphoreTake(ringbuffer.sem_write, portMAX_DELAY);
	if(ret != pdTRUE) {
		return false;
	}
	// taskENTER_CRITICAL();
	ringbuffer.elements[ringbuffer.w_index].prod_id = elem.prod_id;
	ringbuffer.elements[ringbuffer.w_index].prod_value = elem.prod_value;
	ringbuffer.w_index = (ringbuffer.w_index + 1) % RINGBUFFER_SIZE;
	// taskEXIT_CRITICAL();
	ret = xSemaphoreGive(ringbuffer.sem_read);
	if(ret != pdTRUE) {
		return false;
	}
	return true;
}

bool Ringbuffer_Get(rb_elem_t *elem) {
	BaseType_t ret;
	ret = xSemaphoreTake(ringbuffer.sem_read, portMAX_DELAY);
	if(ret != pdTRUE) {
		return false;
	}
	// taskENTER_CRITICAL();
	elem->prod_id = ringbuffer.elements[ringbuffer.r_index].prod_id;
	elem->prod_value = ringbuffer.elements[ringbuffer.r_index].prod_value;
	ringbuffer.r_index = (ringbuffer.r_index + 1) % RINGBUFFER_SIZE;
	// taskEXIT_CRITICAL();
	ret = xSemaphoreGive(ringbuffer.sem_write);
	if(ret != pdTRUE) {
		return false;
	}
	return true;
}

void Ringbuffer_Init() {
	// Annahme das alles funktioniert / Keine Fehlerbehandlung
	ringbuffer.sem_read = xSemaphoreCreateCounting(RINGBUFFER_SIZE, 0);
			//osSemaphoreNew(RINGBUFFER_SIZE, 0, NULL);
	ringbuffer.sem_write = xSemaphoreCreateCounting(RINGBUFFER_SIZE, RINGBUFFER_SIZE);
	ringbuffer.r_index = 0;
	ringbuffer.w_index = 0;
	for(uint8_t i = 0; i < RINGBUFFER_SIZE; i++) {
		ringbuffer.elements[i].prod_id = NULL;
		ringbuffer.elements[i].prod_value = 0;
	}
}
