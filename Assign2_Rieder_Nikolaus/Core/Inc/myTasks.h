/*
 * myTasks.h
 *
 *  Created on: Mar 16, 2023
 *      Author: rini
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_


#include <stdlib.h> // srand() & rand()
#include <string.h>

#include "retarget.h"

#include "ringbuffer.h"
#include "main.h"

extern osMemoryPoolId_t taskPoolHandle;

void ProducerTask(void *argument);
void ConsumerTask(void *argument);
/**
 * @brief Task for starting all producers (runs only once)
 *
 * @param argument ... number of producers needs to be provided as (uint8_t*)
 */
void StartTasks(void *argument);

#endif /* INC_MYTASKS_H_ */
