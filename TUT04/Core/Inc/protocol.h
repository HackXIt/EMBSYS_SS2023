/*
 * protocol.h
 *
 *  Created on: Apr 17, 2023
 *      Author: rini
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_


#define SOF '#' // Start-of-frame
#define EOF '\r' // End-of-frame
#define SEP ';' // Seperator char

// For all of the HAL types and functions
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
/*
#define SOF '#' // Start-of-frame
#define EOF '\n' // End-of-frame
#define SEP ';' // Seperator char
*/
#define UART_TIMEOUT 2000
#define DATA_LIMIT 8
#define DATA_START 3 // DATA starts after SOF,CMD,SEP => 3 characters in
/*
char SOF =  '#'; // Start-of-frame
char EOF = '\n'; // End-of-frame
char SEP = ';'; // Seperator char
*/
bool parseInput(char *input, uint8_t size, int *targetTask, int *targetValue);
bool cmdIsValid(char cmd);
bool cmdDataIsValid(uint8_t size);


#endif /* INC_PROTOCOL_H_ */
