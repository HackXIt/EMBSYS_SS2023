/*
 * ringbuffer.c
 *
 *  Created on: Mar 16, 2023
 *      Author: rini
 */

#include "ringbuffer.h"

/*
 * Theoretically I could have written code that manages multiple ringbuffers on the same shared memory pool.
 * But I figured, one is enough, so I limited myself by using only 1 allocation.
 * Using shared memory is also a bit overkill, but I thought: Meh, why not.
 * It adds possibility to have a ringbuffer_managerTask
 */

// Fixed memory pool name
const osMemoryPoolAttr_t ringbufferPool_attributes = {
		.name = "ringbufferPool",
		.attr_bits = 0
};

// Global variable for a single allocated ringbuffer on the shared memory
ringbuffer_t *rb_allocated = NULL;

bool Ringbuffer_Put(rb_elem_t elem) {
	osStatus_t ret;
	// Check if ringbuffer is initialized
	if(rb_allocated == NULL) {
		Ringbuffer_ErrorHandler(0, "Ringbuffer_Put()", "uninitialized");
		return false;
	}
	// Check semaphores
	if((ret = osSemaphoreAcquire(rb_allocated->sem_write, osWaitForever)) != osOK) {
		Ringbuffer_ErrorHandler(ret, "Ringbuffer_Put()", "acquire (write)");
		return false; // Could not acquire semaphore to lock read in given timeout
	}
	osKernelLock();
	// Write ringbuffer element
	rb_allocated->rb_elements[rb_allocated->wIndex] = elem;
	rb_allocated->wIndex = (rb_allocated->wIndex+1) % RINGBUFFER_SIZE;
	osKernelUnlock();
	// Release semaphores
	if((ret = osSemaphoreRelease(rb_allocated->sem_read)) != osOK) {
		Ringbuffer_ErrorHandler(ret, "Ringbuffer_Put()", "release (read)");
		return false; // Could not release semaphore
	}
	return true;
}

bool Ringbuffer_Get(rb_elem_t *elem) {
	osStatus_t ret;
	// Check if ringbuffer is initialized
	if(rb_allocated == NULL) {
		Ringbuffer_ErrorHandler(0, "Ringbuffer_Get()", "uninitialized");
		return false;
	}
	// Acquire read semaphore
	if((ret = osSemaphoreAcquire(rb_allocated->sem_read, osWaitForever)) != osOK) {
		Ringbuffer_ErrorHandler(ret, "Ringbuffer_Get()", "acquire (read)");
		return false; // Could not acquire semaphore to lock write in given timeout
	}
	osKernelLock();
	// Read ringbuffer element
	*elem = rb_allocated->rb_elements[rb_allocated->rIndex];
	rb_allocated->rIndex = (rb_allocated->rIndex+1) % RINGBUFFER_SIZE;
	osKernelUnlock();
	// Release write semaphore
	if((ret = osSemaphoreRelease(rb_allocated->sem_write)) != osOK) {
		Ringbuffer_ErrorHandler(ret, "Ringbuffer_Get()", "release (write)");
		return false; // Could not release semaphore
	}
	return true;
}

osMemoryPoolId_t Ringbuffer_Init(uint8_t producer_count) {
	osMemoryPoolId_t newPool = osMemoryPoolNew(1, sizeof(ringbuffer_t), &ringbufferPool_attributes);
	if(newPool == NULL) {
		Ringbuffer_ErrorHandler(0, "Ringbuffer_Init()", "Failed to create new memory pool");
	}
	rb_allocated = (ringbuffer_t*) osMemoryPoolAlloc(newPool, RINGBUFFER_TIMEOUT);
	/*
	 * According to the assignment, there will always be only 1 consumer.
	 * This means the read semaphore is always binary (thus max_count=1).
	 * I assume, that the consumer initially needs to wait (thus initial_count=0)
	 */
	rb_allocated->sem_read = osSemaphoreNew(RINGBUFFER_SIZE, 0, NULL);
	if(rb_allocated->sem_read == NULL) {
		Ringbuffer_ErrorHandler(0, "Ringbuffer_Init()", "Failed to create 'read' semaphore");
	}
	/*
	 * In the assignment, there will always be NUM_PRODS producers.
	 * This means the write semaphore is equal to NUM_PRODS (thus max_count=NUM_PRODS).
	 */
	rb_allocated->sem_write = osSemaphoreNew(RINGBUFFER_SIZE, 1, NULL);
	if(rb_allocated->sem_write == NULL) {
		Ringbuffer_ErrorHandler(0, "Ringbuffer_Init()", "Failed to create 'write' semaphore");
	}
	return newPool;
}

osStatus_t Ringbuffer_Free(osMemoryPoolId_t ringbuffer) {
	return osMemoryPoolFree(ringbuffer, rb_allocated);
}

osStatus_t Ringbuffer_Delete(osMemoryPoolId_t ringbuffer) {
	return osMemoryPoolDelete(ringbuffer);
}

void Ringbuffer_ErrorHandler(osStatus_t status, char * function, char * msg) {
	// Only output to UART in DEBUG mode
	// If this gets activated, the producer stack-size must be changed!!!
#ifdef DEBUG_RB
	printf("%s => %s ", function, msg);
	switch(status) {
		case osErrorTimeout:
			printf("(osErrorTimeout)");
			break;
		case osErrorResource:
			printf("(osErrorResource)");
			break;
		case osErrorParameter:
			printf("(osErrorParameter)");
			break;
		case osErrorNoMemory:
			printf("(osErrorNoMemory)");
			break;
		default:
			break;
	}
	printf("\r\n");
#endif
}
