/*
 * ringbuffer.c
 *
 *  Created on: Mar 20, 2023
 *      Author: rini
 */

#include "ringbuffer.h"

static ringbuffer_t ringbuffer;

bool Ringbuffer_Put(rb_elem_t elem) {
	osStatus_t ret;
	ret = osSemaphoreAcquire(ringbuffer.sem_write, osWaitForever);
	if(ret != osOK) {
		return false;
	}
	// osKernelLock();
	ringbuffer.elements[ringbuffer.w_index].prod_id = elem.prod_id;
	ringbuffer.elements[ringbuffer.w_index].prod_value = elem.prod_value;
	ringbuffer.w_index = (ringbuffer.w_index + 1) % RINGBUFFER_SIZE;
	// osKernelUnlock();
	ret = osSemaphoreRelease(ringbuffer.sem_read);
	if(ret != osOK) {
		return false;
	}
	return true;
}

bool Ringbuffer_Get(rb_elem_t *elem) {
	osStatus_t ret;
	ret = osSemaphoreAcquire(ringbuffer.sem_read, osWaitForever);
	if(ret != osOK) {
		return false;
	}
	// osKernelLock();
	elem->prod_id = ringbuffer.elements[ringbuffer.r_index].prod_id;
	elem->prod_value = ringbuffer.elements[ringbuffer.r_index].prod_value;
	ringbuffer.r_index = (ringbuffer.r_index + 1) % RINGBUFFER_SIZE;
	// osKernelUnlock();
	ret = osSemaphoreRelease(ringbuffer.sem_write);
	if(ret != osOK) {
		return false;
	}
	return true;
}

void Ringbuffer_Init() {
	// Annahme das alles funktioniert / Keine Fehlerbehandlung
	ringbuffer.sem_read = osSemaphoreNew(RINGBUFFER_SIZE, 0, NULL);
	ringbuffer.sem_write = osSemaphoreNew(RINGBUFFER_SIZE, RINGBUFFER_SIZE, NULL);
	ringbuffer.r_index = 0;
	ringbuffer.w_index = 0;
	for(uint8_t i = 0; i < RINGBUFFER_SIZE; i++) {
		ringbuffer.elements[i].prod_id = NULL;
		ringbuffer.elements[i].prod_value = 0;
	}
}
