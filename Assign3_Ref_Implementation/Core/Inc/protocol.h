/*
 * protocol.h
 *
 *  Created on: Apr 22, 2023
 *      Author: rini
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief A simple object containing the task number and interval parsed by the protocol
 * @note The protocol does not handle figuring out which task this corresponds to.
 */
typedef struct parsedObj
{
	uint8_t taskNumber;
	uint32_t targetInterval;
}parsedObj_t;

bool parseInput(char *input, parsedObj_t *parsedObj);

#endif /* INC_PROTOCOL_H_ */
