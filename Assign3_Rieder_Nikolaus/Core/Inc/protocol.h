/*
 * protocol.h
 *
 *  Created on: Apr 16, 2023
 *      Author: rini
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"

#define TASK_NUM_POS	1
#define INPUT_SEPERATOR	" "
#define SOF				'T'


/*
 * A few requirements for this module:
 * The protocol should handle the UART2 communication
 * It receives commands in the form of "T2 1000ms" to change the update interval of producers
 * The 'T2' refers to task 2 in the example
 * The '1000ms' refers to the interval time to set
 *
 * All this module does, is receive UART input, parse it and output correct values from it.
 */

/**
 * @brief A simple object containing the parsed task number and interval
 * @note The protocol does not handle figuring out which task this corresponds to.
 */
typedef struct taskInfoObj
{
	uint8_t taskNumber;
	uint32_t targetInterval;
}taskInfoObj_t;

/**
 * @brief A status enumeration for the parser functions
 *
 */
typedef enum parseStatus
{
	parseOK,       /**< parseOK */
	parseException,/**< parseException */
	parseFailure   /**< parseFailure */
}parseStatus_t;

/**
 * @brief A function which parses a given input and writes the result to a given object-pointer
 *
 * @param input ... string to parse
 * @param size ... length of string
 * @param parsedObj ... pointer to output object
 * @return ... true on success (provided object was changed), false on failure (provided object remains unchanged)
 */
parseStatus_t parseInput(char *input, taskInfoObj_t *parsedObj);

#endif /* INC_PROTOCOL_H_ */
