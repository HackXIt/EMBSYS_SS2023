/*
 * protocol.c
 *
 *  Created on: Apr 16, 2023
 *      Author: rini
 */
#include "protocol.h"

parseStatus_t parseInput(char *input, taskInfoObj_t *parsedObj) {
	if(parsedObj == NULL || input == NULL) return parseException;
	int num = 0;
	int interval = 0;
	int ret = sscanf(input, "T%d %dms\r", &num, &interval);
	switch(ret) {
	case 0:
		return parseFailure;
	case 1:
		return parseFailure;
	case 2:
		parsedObj->taskNumber = num;
		parsedObj->targetInterval = interval;
		return parseOK;
	default:
		// TODO Undefined/default behavior in parseInput?
		return parseFailure;
	}
}
	/*
	// Validate "Start-of-frame" (SOF)
	if(input[0] != SOF) {
		return wrongSOF;
	}
	// Validate "End-of-frame" (EOF)
	char *eof = NULL;
	if((eof = strstr(input, "\r")) == NULL) {
		return wrongEOF;
	}
	// Validate input format
	if(!taskIsValid(input[TASK_NUM_POS])) {
		return invalidInput;
	}
	// Only single digit task is supported
	parsedObj->taskNumber = (uint8_t)input[TASK_NUM_POS] - '0'; // Converts char number to real number
	// Parse targetInterval
	char *sep = NULL;
	if((sep = strstr(packetData, INPUT_SEPERATOR)) == NULL) {
		return invalidInput;
	}
	// posOfSeperator = sep - input;
	// posOfEnd = eof - input;
	// posOfEnd - posOfSeperator => size of input parameter, including space for \0
	uint8_t sizeOfParameter = ((sep - input) - (eof - input));
	char *targetInterval = pvPortMalloc(sizeof(char) * sizeOfParameter);
	if(targetInterval == NULL) {
		return memoryError;
	}
	strncpy(targetInterval, input+(sep - input + 1), sizeOfParameter);
	// End with NUL terminator instead of \r
	targetInterval[sizeOfParameter-1] = '\0';
	uint8_t targetParameter;
	uint8_t ret = sscanf(targetInterval, "%dms", &targetParameter);
	vPortFree(targetInterval);
	if(ret == EOF) {
		return invalidParameter;
	} else if (ret != sizeOfParameter) {
		return invalidParameter;
	}
	parsedObj->targetInterval = targetParameter;
	return parseOK;
	*/
