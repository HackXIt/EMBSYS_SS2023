/*
 * taskHelper.h
 *
 *  Created on: Apr 20, 2023
 *      Author: rini
 */

#ifndef INC_TASKHELPER_H_
#define INC_TASKHELPER_H_

#include "myTasks.h"

#include <stdbool.h>
#include <stdio.h>

#ifdef USE_CMSIS
#include <cmsis_os2.h>

/**
 * @brief Checks the created handle for null and prints appropriate message along with the provided customMsg
 *
 * @param handle ... thread handle to verify
 * @param customMsg ... custom message to print
 */
void verifyTaskCreation(osThreadId_t handle, char * customMsg);

/**
 * @brief This little function initializes an osThreadAttr_t variable in a re-usable manner
 * It uses the documented default values for: cb_mem, cb_size and stack_mem
 * @note The function is supposed to be used when the osThreadAttr_t is only declared but not directly initialized
 *
 * @param attribute ... pointer to the variable to be initialized
 * @param name ... of the thread
 * @param stack_size ... of the thread
 * @param priority ... of the thread
 * @param attr_bits ... of the thread
 */
void initAttributes(osThreadAttr_t *attribute, const char *name, uint32_t stack_size, osPriority_t priority, uint32_t attr_bits);

/**
 * @brief Wrapper function to handle various possible failure states of event flag functions
 *
 * @param flag_return ... return value of the event flag function
 * @return ... the provided argument is returned, when no failure state was handled
 */
uint32_t handleFlagReturn(uint32_t flag_return);

/**
 * @brief Wrapper function to handle various possible failure states from CMSIS functions
 *
 * @param status ... return-value of the CMSIS function
 * @return true on osOK, false on any other state
 */
bool handleOSStatus(osStatus_t status);

#endif /* USE_CMSIS DEFINED */

#ifndef USE_CMSIS
#include <FreeRTOS.h>

/**
 * @brief Checks the created handle for null and prints appropriate message along with the provided customMsg
 *
 * @param handle ... thread handle to verify
 * @param customMsg ... custom message to print
 */
void verifyTaskCreation(BaseType_t return_value, char * customMsg);

#endif /* USE_CMSIS NOT-DEFINED */

#endif /* INC_TASKHELPER_H_ */
