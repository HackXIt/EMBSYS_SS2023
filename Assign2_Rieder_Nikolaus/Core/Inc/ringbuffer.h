/*
 * ringbuffer.h
 *
 *  Created on: Mar 16, 2023
 *      Author: rini
 */

#ifndef INC_RINGBUFFER_H_
#define INC_RINGBUFFER_H_

#include <stdbool.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"

//#define DEBUG_RB

#ifdef DEBUG_RB
#include "retarget.h"
#endif

// Adjustable ringbuffer size (has no impact on producers)
#define RINGBUFFER_SIZE				10
#define RINGBUFFER_TIMEOUT			5000

// definition of my chosen element type, since the assignment description did not provide it.
typedef struct rb_element {
	osThreadId_t producer_id;
	uint8_t producer_value;
}rb_elem_t;

typedef struct ringbuffer {
	rb_elem_t rb_elements[RINGBUFFER_SIZE];
	osSemaphoreId_t sem_read;
	osSemaphoreId_t sem_write;
	uint8_t rIndex;
	uint8_t wIndex;
}ringbuffer_t;

// Functions that are required by the assignment
/**
 * @brief Put element in ringbuffer
 *
 * @param elem ... element to add in ringbuffer
 * @return true on success, false on failure (i.e. elem was not added)
 */
bool Ringbuffer_Put(rb_elem_t elem);
/**
 * @brief Get element from ringbuffer
 *
 * @param elem ... pointer to store received element to
 * @return true on success, false on failure (i.e. pointer will be NULL)
 */
bool Ringbuffer_Get(rb_elem_t *elem);

// Functions I added for simplicity
/**
 * @brief Initialize the default & single ringbuffer
 *
 * @return ... the memoryPoolId of the ringbuffer is returned
 */
osMemoryPoolId_t Ringbuffer_Init(uint8_t producer_count);
/**
 * @brief Wrapper for osMemoryPoolFree to free the default & single ringbuffer
 *
 * @param ringbuffer ... the memoryPoolId of the initialized ringbuffer
 * @return ... the status of the operation returned by FreeRTOS
 */
osStatus_t Ringbuffer_Free(osMemoryPoolId_t ringbuffer);
/**
 * @brief Wrapper for osMemoryPoolDelete to free the default & single ringbuffer
 *
 * @param ringbuffer ... the memoryPoolId of the initialized ringbuffer
 * @return ... the status of the operation returned by FreeRTOS
 */
osStatus_t Ringbuffer_Delete(osMemoryPoolId_t ringbuffer);

void Ringbuffer_ErrorHandler(osStatus_t status, char * function, char * msg);

#endif /* INC_RINGBUFFER_H_ */
