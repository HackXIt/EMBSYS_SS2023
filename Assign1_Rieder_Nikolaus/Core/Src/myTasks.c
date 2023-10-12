/*
 * myTasks.c
 *
 *  Created on: Mar 4, 2023
 *      Author: rini
 */

/*
 * Assignment description/requirements:
 * - Allocate memory on the stack (in 3 different tasks)
 * - Periodically output current and remaining stack size of the 3 tasks in a fourth task
 * - Stack sizes should be read using global osThreadAttr_t structs and osThreadGetStackSpace()
 * - Output should be done via UART interface
 * - Tasks and queues must be allocated statically in this assignment
 * - Investigate and document findings in regards to changing the code generation to "DYNAMIC"
 */

// Assignment requirements are faulty, because it presumes all allocation is static

#include <myTasks.h>

/*
 * Diese Werte könnte ich prinzipiell mit rand() zufällig gestalten.
 * Ich habe mich entschlossen diese statisch zu machen, damit die Unterschiede schneller und deutlicher erkennbar sind.
 * Die Unterschiede sind somit lediglich abhängig vom Inhalt der Task-Funktionen.
 */
#define TASK1VALUES			5
#define TASK2VALUES			10
#define TASK3VALUES			15

/**
 * @brief Allocate memory on the stack (malloc)
 *
 * @param argument
 */
void AllocTask1(void *argument) {
	uint8_t values_on_stack[TASK1VALUES]; //
	uint8_t *values_on_heap;
	for(;;) {
		for(uint8_t i = 0; i < TASK1VALUES; i++) {
			values_on_stack[i] = 0xAA;
		}
		values_on_heap = (uint8_t*)pvPortMalloc(TASK1VALUES * sizeof(uint8_t));
		vTaskDelay(TASK1VALUES * 1000);
		for(uint8_t i = 0; i < TASK1VALUES; i++) {
			values_on_heap[i] = values_on_stack[i] + 1;
			/*
			 * Das zusätzliche Delay in diesem Task hilft bei der frühzeitigen Erkennung des Unterschieds.
			 * Beim dynamic allocation model wird durch die Recursion deutlich mehr Heap-Speicher angefragt/verwendet.
			 */
			vTaskDelay(TASK1VALUES * 1000);
		}
		vTaskDelay(TASK1VALUES * 1000);
		vPortFree(values_on_heap);
	}
}

/**
 * @brief Allocate memory on the stack (recursion)
 *
 * @param argument
 */
void AllocTask2(void *argument) {
	uint8_t values_on_stack[TASK2VALUES]; //
	uint8_t *values_on_heap;
	for(;;) {
		for(uint8_t i = 0; i < TASK2VALUES; i++) {
			values_on_stack[i] = 0xBB;
		}
		values_on_heap = (uint8_t*)pvPortMalloc(TASK2VALUES * sizeof(uint8_t));
		vTaskDelay(TASK2VALUES * 1000);
		for(uint8_t i = 0; i < TASK2VALUES; i++) {
			values_on_heap[i] = values_on_stack[i] + 1;
		}
		vTaskDelay(TASK2VALUES * 1000);
		vPortFree(values_on_heap);
	}
}

/**
 * @brief Allocate memory on the stack (etc.)
 *
 * @param argument
 */
void AllocTask3(void *argument) {
	uint8_t values_on_stack[TASK3VALUES]; //
	uint8_t *values_on_heap;
	for(;;) {
		for(uint8_t i = 0; i < TASK3VALUES; i++) {
			values_on_stack[i] = 0xCC;
		}
		values_on_heap = (uint8_t*)pvPortMalloc(TASK3VALUES * sizeof(uint8_t));
		vTaskDelay(TASK3VALUES * 1000);
		for(uint8_t i = 0; i < TASK3VALUES; i++) {
			values_on_heap[i] = values_on_stack[i] + 1;
		}
		vTaskDelay(TASK3VALUES * 1000);
		vPortFree(values_on_heap);
	}
}

/**
 * @brief Periodically output current and remaining stack size of tasks
 * The task should use the global structs (osThreadAttr_t) and osThreadGetStackSpace()
 * @param argument
 */
void PollStackSize(void *argument) {
	/*
	 * Set STDOUT buffer to NULl => Always flush!
	 * Doesn't work in STM32 for some reason
	 */
	//setbuf(stdout, NULL);
	HeapStats_t heapStats;
	// I could have used xTaskGetHandle("...") but this was simpler
	void *handles[3] = {allocTask1Handle, allocTask2Handle, allocTask3Handle};
	TaskStatus_t taskInfos[3];

	// Irrelevant variables used during development
	//const osThreadAttr_t *attributes[3] = {&allocTask1_attributes, &allocTask2_attributes, &allocTask3_attributes};
	//uint32_t stackHighWaterMark[3];

	// Initial information
	for(uint8_t i = 0; i < 3; i++) {
		taskInitialSpace(handles[i]);
	}
	for(;;) {
		/*
		 * Alternative way of getting the remaining stack space of the tasks:
		stackHighWaterMark[0] = uxTaskGetStackHighWaterMark(allocTask1Handle) * 4;
		stackHighWaterMark[1] = uxTaskGetStackHighWaterMark(allocTask2Handle) * 4;
		stackHighWaterMark[2] = uxTaskGetStackHighWaterMark(allocTask3Handle) * 4;
		*/
		vPortGetHeapStats(&heapStats);
		heapInfoOutput(&heapStats);
		printf("TASKS:\r\n");
		printf("%-15s%-15s%-15s%-15s%-15s\r\n", "Task#", "TaskName", "TaskState", "TaskPriority", "RemainingStackSpace");
		for(uint8_t i = 0; i < 3; i++) {
			vTaskGetInfo(handles[i], &taskInfos[i], pdTRUE, eInvalid);
			taskInfoOutput(&taskInfos[i]);
		}
		printf("\r\n");
		/*
		 * printf() is sending to STDOUT, which is buffered.
		 * Need to flush, because '\r' is added implicitly inside retarget.c
		 * Alternative would be to always write "\r\n" when using printf()
		 * I decided to always write "\r\n" and commented out the implicit '\r' in retarget.c
		 */
		// fflush(stdout);
		vTaskDelay(2000);
	}
}

/* Implementations for arbitrary functions used in tasks */

void taskInitialSpace(osThreadId_t taskHandle) {
	printf("Initial space of %s: %u\r\n", osThreadGetName(taskHandle), (unsigned int)osThreadGetStackSpace(taskHandle));
}

void taskInfoOutput(TaskStatus_t *taskStatus) {
	printf("%-15lu%-15s", taskStatus->xTaskNumber, taskStatus->pcTaskName);
	switch(taskStatus->eCurrentState) {
		case eRunning:
			printf("%-15s", "RUNNING");
			break;
		case eReady:
			printf("%-15s", "READY");
			break;
		case eBlocked:
			printf("%-15s", "BLOCKED");
			break;
		case eSuspended:
			printf("%-15s", "SUSPENDED");
			break;
		case eDeleted:
			printf("%-15s", "DELETED");
			break;
		case eInvalid:
			printf("%-15s", "INVALID");
			break;
		default:
			printf("%-15s", "UNKNOWN");
			break;
	}
	printf("%-15lu", taskStatus->uxCurrentPriority);
	printf("%-15u", taskStatus->usStackHighWaterMark);
	printf("\r\n");
}

void heapInfoOutput(HeapStats_t *heapStats) {
	printf("HEAP:\r\n");
	printf("%-20s%-20s%-20s%-20s%-20s%-20s%-20s\r\n", "AvailableHeapBytes", "LargestFreeBlock", "SmallestFreeBlock", "#FreeBlocks", "MinRemainingBytes", "#Allocations", "#Frees");
	printf("%-20u", heapStats->xAvailableHeapSpaceInBytes);
	printf("%-20u", heapStats->xSizeOfLargestFreeBlockInBytes);
	printf("%-20u", heapStats->xSizeOfSmallestFreeBlockInBytes);
	printf("%-20u", heapStats->xNumberOfFreeBlocks);
	printf("%-20u", heapStats->xMinimumEverFreeBytesRemaining);
	printf("%-20u", heapStats->xNumberOfSuccessfulAllocations);
	printf("%-20u", heapStats->xNumberOfSuccessfulFrees);
	printf("\r\n");
}

int factorial(int n) {
	if(n == 0) {
		return 1;
	} else {
		return n * factorial(n-1);
	}
}

int fibbonacci(int n) {
	if(n == 0) {
		return 0;
	} else if (n == 1) {
		return 1;
	} else {
		return (fibbonacci(n-1) + fibbonacci(n-2));
	}
}

void taskErrorHandler(osThreadId_t id, char* err_msg) {
	printf("Error on Thread: %p\n", id);
	printf(err_msg);
	for(;;) {
		osDelay(1);
	}
}
