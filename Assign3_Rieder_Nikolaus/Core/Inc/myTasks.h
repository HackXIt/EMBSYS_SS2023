/*
 * myTasks.h
 *
 *  Created on: Apr 16, 2023
 *      Author: rini
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_

// Comment this, to use the FreeRTOS variant instead of CMSIS
/*
 * NOTE: The FreeRTOS variant of the application is currently broken and goes into hardFault
 * I prefer CMSIS anyways and I tried making a direct clone in pure FreeRTOS API for good comparison
 * But apparently I get some kind of access violation or something, which triggers a hardFault.
 */
#define USE_CMSIS

// -------- FreeRTOS & STM32 headers
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "usart.h"
#ifndef USE_CMSIS
#include "queue.h"
#include "event_groups.h"
#endif
// -------- Custom headers
#include "protocol.h"
#include "taskHelper.h"

// -------- Defines
#define STARTING_INTERVAL	1000U
#define UPDATE_TIMEOUT		10000
#define NUM_PRODUCERS	3
#define MSG_BUF_SIZE	100
// Application event flags
// DON'T USE THESE BITS:0bXXXXXXXX000000000000000000000000
// They are not used in FreeRTOS and will cause problems.
// monitor flags
#define RX_BYTE			0b00000000000000000000000000000001
#define INPUT			0b00000000000000000000000000000010
#define QUEUE_UPDATE	0b00000000000000000000000000000100
// producer flags
#define TASK_UPDATE		0b00000000000000000000000000001000
#define TASK_INCREMENT	0b00000000000000000000000000010000
// ... more flags if necessary ...
/*
#define FLAG6			0b00000000000000000000000000100000
#define FLAG7			0b00000000000000000000000001000000
#define FLAG8			0b00000000000000000000000010000000
#define FLAG9			0b00000000000000000000000100000000
*/

// -------- Macros
#define SIZEOF(arr) sizeof(arr) / sizeof(*arr) //src: https://www.geeksforgeeks.org/using-sizof-operator-with-array-paratmeters-in-c/

// -------- Structures
/**
 * @brief A data object for the updateQueue
 *
 */
typedef struct dataObj
{
#ifdef USE_CMSIS
	osThreadId_t taskHandle;
#endif
#ifndef USE_CMSIS
	TaskHandle_t taskHandle;
#endif
	uint32_t value;
}dataObj_t;

/**
 * @brief A producer object, containing producerNumber and necessary queue pointers for operation
 *
 */
typedef struct producerObj
{
	uint8_t producerNumber;
#ifdef USE_CMSIS
	osMessageQueueId_t *monitorQueue; // for TX-Data
	osMessageQueueId_t *updateQueue; // for RX-Data
#endif
#ifndef USE_CMSIS
	QueueHandle_t *monitorQueue;
	QueueHandle_t *updateQueue;
#endif
}producerObj_t;

/**
 * @brief An initializer sets up necessary variables and creates application threads
 *
 * @param argument (not used)
 */
void StartInitializeTask(void *argument);


// -------- Function prototypes
/**
 * @brief A producer updates it private counter variable and sends the value to a message queue
 *
 * @note The counter variable is incremented periodically
 * @param argument ... provide osEventFlagsId_t of the producer
 */
void ProducerTask(void *argument);

/**
 * @brief The monitor reads from UART and outputs values from a message queue
 *
 * @note The monitor updates producer intervals based on the command input parsed in protocol
 * @param argument ... provider taskObj_t array, containing all producers
 */
void MonitorTask(void *argument);


#endif /* INC_MYTASKS_H_ */
