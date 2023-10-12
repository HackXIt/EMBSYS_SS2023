/*
 * protocol.c
 *
 *  Created on: Apr 17, 2023
 *      Author: rini
 */


/*
 * uart_protocol.c
 *
 *  Created on: Nov 5, 2022
 *      Author: rini
 */

#include "protocol.h"

char validCmd[] = "L";
char cmd = '\0';
char cmdData[DATA_LIMIT];

bool parseInput(char *input, uint8_t size, int *targetTask, int *targetValue) {

	// Validate SOF
	/*
	if (input[0] != SOF) {
		return false;
	}
	// Validate EOF
	char *eof = NULL;
	if((eof = strstr(input, "\r")) == NULL) {
		return false;
	}
	// Validate CMD
	if(cmdIsValid(input[1])) {
		cmd = input[1];
	} else {
		return false;
	}

	uint8_t targetValue = input[2] - '0';
	// Validate SEP

	// CMD: T1 1000ms => "T1 1000ms\r\n\0"
	// 1 => TargetTask
	// 1000 => TargetValue
	// T == [0] sozusagen der SOF
	// 1 == [1] => Direkt abspeichern als Wert
	// " " == [2] => SEP
	// "..." == TargetValue
	// "ms"
	// "\r" == EOF
	char *sep = NULL;
	if((sep = strstr(input, " ")) == NULL) {
		return false;
	}
	uint8_t posOfSeperator = sep - input;
	// Could implement check => posOfSeperator == DATA_START
	//uint8_t posOfPacketEnd = eof - packetData;
	//uint8_t sizeOfCmdData = posOfPacketEnd - posOfSeperator - 1; // End-Adresse - Start-Adresse - FIXWERT => Größe von CMD-DATA
	// Copy CMD_DATA
	//strncpy(cmdData, input+(posOfSeperator+1), sizeOfCmdData);

	*/
	uint8_t ret = sscanf(input, "T%d %dms\r\n", targetTask, targetValue);
	// TODO validate sscanf
	// Validate DATA
	return true;
}

bool cmdIsValid(char cmd) {
	for(uint8_t i = 0; i < strlen(validCmd); i++) {
		if(validCmd[i] == cmd) {
			return true;
		}
	}
	return false;
}

bool cmdDataIsValid(uint8_t size) {
	if(size > DATA_LIMIT) {
		return false;
	}
	return true;
}
