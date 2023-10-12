/*
 * myTasks.h
 *
 *  Created on: Mar 30, 2023
 *      Author: RINI
 */

#ifndef INC_MYTASKS_H_
#define INC_MYTASKS_H_

#define USE_CMSIS

/*
#ifndef USE_CMSIS
#include "FreeRTOS.h"
#endif
#ifdef USE_CMSIS
#include "cmsis_os2.h"
#endif
*/

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "usart.h"


#include <string.h>


void InitTasks(void *argument);

void ReceiverTask(void *argument);
void SenderTask(void *argument);
void MonitorTask(void *argument);

#endif /* INC_MYTASKS_H_ */
