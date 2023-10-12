/*
 * taskHelper.c
 *
 *  Created on: Apr 20, 2023
 *      Author: rini
 */

#include "taskHelper.h"

#ifdef USE_CMSIS

void verifyTaskCreation(osThreadId_t handle, char * customMsg) {
	if(handle == NULL) {
		printf("Failed to create thread: %s\r\n", customMsg);
	} else {
		printf("Created thread: %s\r\n", customMsg);
	}
}

void initAttributes(osThreadAttr_t *attribute, const char *name, uint32_t stack_size, osPriority_t priority, uint32_t attr_bits) {
	if(attribute == NULL) return;
	attribute->name = name;
	attribute->stack_size = stack_size;
	attribute->priority = priority;
	attribute->attr_bits = attr_bits;
	// Initialize default values for static allocation
	attribute->cb_mem = NULL;
	attribute->cb_size = 0;
	attribute->stack_mem = NULL;
}

uint32_t handleFlagReturn(uint32_t flag_return) {
	switch(flag_return) {
	case osFlagsError:
		// TODO handle osFlagsError
		break;
	case osFlagsErrorUnknown:
		// TODO handle osFlagsErrorUnknown
		break;
	case osFlagsErrorTimeout:
		// TODO handle osFlagsErrorTimeout
		break;
	case osFlagsErrorResource:
		// TODO handle osFlagsErrorResource
		break;
	case osFlagsErrorParameter:
		// TODO handle osFlagsErrorParameter
		break;
	case osFlagsErrorISR:
		// TODO handle osFlagsErrorISR
		break;
	default:
		// TODO default handling behavior
		break;
	}
	// If no flag error occurred, then returned flag state may be used by caller
	return flag_return;
}

bool handleOSStatus(osStatus_t status) {
	switch(status) {
	case osOK:
		return true;
	case osError:
		// TODO handle osError
		break;
	case osErrorTimeout:
		// TODO handle osErrorTimeout
		break;
	case osErrorResource:
		// TODO handle osErrorResource
		break;
	case osErrorParameter:
		// TODO handle osErrorParameter
		break;
	case osErrorNoMemory:
		// TODO handle osErrorNoMemory
		break;
	case osErrorISR:
		// TODO handle osErrorISR
		break;
	default:
		break;
	}
	return false;
}

#endif

#ifndef USE_CMSIS

void verifyTaskCreation(BaseType_t return_value, char * customMsg) {
	if(return_value == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY) {
		printf("Failed to create thread: %s\r\n", customMsg);
	} else {
		printf("Created thread: %s\r\n", customMsg);
	}
}

#endif
