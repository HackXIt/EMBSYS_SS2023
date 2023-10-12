/*
 * myTasks.h
 *
 *  Created on: Mar 20, 2023
 *      Author: rini
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os2.h"
#include "ringbuffer.h"
#include "usart.h"

#define NUM_PRODS		2

void StartTasks(void *argument);
void ConsumerTask(void *argument);
void ProducerTask(void *argument);

#endif /* INC_MYTASKS_H_ */
