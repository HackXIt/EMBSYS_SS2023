/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_RX_MAX_DELAY						1000
#define UART_TX_MAX_DELAY						10000
#define	UART_RX_BUFFER_SIZE						255
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static uint8_t *msgBuffer = NULL;
static volatile uint8_t msg_flag = 0;

/* USER CODE END Variables */
/* Definitions for readUART_task */
osThreadId_t readUART_taskHandle;
const osThreadAttr_t readUART_task_attributes = {
  .name = "readUART_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for writeUART_task */
osThreadId_t writeUART_taskHandle;
const osThreadAttr_t writeUART_task_attributes = {
  .name = "writeUART_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void ReadUART(void *argument);
void WriteUART(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of readUART_task */
  readUART_taskHandle = osThreadNew(ReadUART, NULL, &readUART_task_attributes);

  /* creation of writeUART_task */
  writeUART_taskHandle = osThreadNew(WriteUART, NULL, &writeUART_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_ReadUART */
/**
  * @brief  Function implementing the readUART_task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ReadUART */
void ReadUART(void *argument)
{
  /* USER CODE BEGIN ReadUART */
  /* Infinite loop */
	uint8_t rxBuffer[UART_RX_BUFFER_SIZE]; // static buffer
	memset(rxBuffer, 0, UART_RX_BUFFER_SIZE);
	uint8_t rxIndex = 0;
	HAL_StatusTypeDef ret;
	char *rx_error_str = "RX Buffer overflow!\r\n";
	char *mem_error_str = "Out of memory!\r\n";
  for(;;)
  {
	  ret = HAL_UART_Receive(&huart2, rxBuffer+rxIndex, 1, UART_RX_MAX_DELAY);
	  if(rxBuffer[rxIndex] == '\r') {
		  msgBuffer = (uint8_t*)pvPortMalloc(rxIndex+3 * sizeof(uint8_t));
		  if(msgBuffer == NULL) {
			  vTaskSuspend(writeUART_taskHandle);
			  HAL_UART_Transmit(&huart2, (uint8_t*)mem_error_str, strlen(mem_error_str), UART_TX_MAX_DELAY);
			  memset(rxBuffer, 0, rxIndex);
			  rxIndex = 0;
			  vTaskResume(writeUART_taskHandle);
			  continue;
		  }
		  memcpy(msgBuffer, rxBuffer, rxIndex);
		  msgBuffer[rxIndex] = '\r';
		  msgBuffer[rxIndex+1] = '\n';
		  msgBuffer[rxIndex+2] = '\0';
		  msg_flag = 1;
		  memset(rxBuffer, 0, UART_RX_BUFFER_SIZE);
		  rxIndex = 0;
	  } else if(ret == HAL_OK) {
		  rxIndex++;
		  if(rxIndex >= UART_RX_BUFFER_SIZE) {
			  vTaskSuspend(writeUART_taskHandle);
			  HAL_UART_Transmit(&huart2, (uint8_t*)rx_error_str, strlen(rx_error_str), UART_TX_MAX_DELAY);
			  HAL_UART_Transmit(&huart2, rxBuffer, rxIndex, UART_TX_MAX_DELAY);
			  memset(rxBuffer, 0, UART_RX_BUFFER_SIZE);
			  rxIndex = 0;
			  vTaskResume(writeUART_taskHandle);
		  }
	  }
  }
  /* USER CODE END ReadUART */
}

/* USER CODE BEGIN Header_WriteUART */
/**
* @brief Function implementing the writeUART_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_WriteUART */
void WriteUART(void *argument)
{
  /* USER CODE BEGIN WriteUART */
  /* Infinite loop */
  for(;;)
  {
    if(msg_flag) {
    	HAL_UART_Transmit(&huart2, msgBuffer, strlen((char*)msgBuffer), UART_TX_MAX_DELAY);
    	vPortFree(msgBuffer);
    	msgBuffer = NULL;
    	msg_flag = 0;
    }
    osDelay(1);
  }
  /* USER CODE END WriteUART */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

