/*
 * protocol.c
 *
 *  Created on: Apr 22, 2023
 *      Author: rini
 */

#include "protocol.h"

bool parseInput(char *input, parsedObj_t *parsedObj) {
	unsigned int parsedNum;
	uint32_t parsedInterval;
	uint8_t ret = sscanf(input, "T%u %lums", &parsedNum, &parsedInterval);
	switch(ret) {
	case 0:
		// 0 arguments were filled => wrong input
		return false;
	case 1:
		// 1 argument was filled => wrong input
		return false;
	case 2:
		// 2 arguments were filled => assume correct input
		parsedObj->taskNumber = parsedNum;
		parsedObj->targetInterval = parsedInterval;
		return true;
	default:
		// Any other case result => wrong input
		return false;
	}
}
