/*
 * myTasks.h
 *
 *  Created on: Apr 17, 2023
 *      Author: rini
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "protocol.h"
#include <stdbool.h>

void MonitorTask(void *argument);

#endif /* INC_MYTASKS_H_ */
