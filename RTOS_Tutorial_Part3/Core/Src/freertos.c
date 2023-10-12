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
#include <ctype.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define UART_RX_BUFFER_SIZE			6
#define UART_TX_BUFFER_SIZE			255
#define UART_MAX_DELAY				5000
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static uint16_t ledDelay = 500;

/* USER CODE END Variables */
/* Definitions for UART_task */
osThreadId_t UART_taskHandle;
const osThreadAttr_t UART_task_attributes = {
  .name = "UART_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for blinkLED_task */
osThreadId_t blinkLED_taskHandle;
const osThreadAttr_t blinkLED_task_attributes = {
  .name = "blinkLED_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void UARTThread(void *argument);
void BlinkLED(void *argument);

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
  /* creation of UART_task */
  UART_taskHandle = osThreadNew(UARTThread, NULL, &UART_task_attributes);

  /* creation of blinkLED_task */
  blinkLED_taskHandle = osThreadNew(BlinkLED, NULL, &blinkLED_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_UARTThread */
/**
  * @brief  Function implementing the UART_task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_UARTThread */
void UARTThread(void *argument)
{
  /* USER CODE BEGIN UARTThread */
  /* Infinite loop */
	char txBuffer[UART_TX_BUFFER_SIZE] = "Running RTOS on STM32...\r\nEnter LED Delay (0 - 65535):\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, strlen(txBuffer), UART_MAX_DELAY);
	char rxBuffer[UART_RX_BUFFER_SIZE] = "";
	uint8_t rxByte;
	uint8_t rxIndex = 0;
	unsigned int tmpDelay = 0;
	for(;;)
	{
	  HAL_UART_Receive(&huart2, &rxByte, 1, HAL_MAX_DELAY);
	  if((char)rxByte == '\r') {
		  // Suspend blinking Thread, so we can change delay without Interruption
		  osThreadSuspend(blinkLED_taskHandle);
		  // Convert integer
		  tmpDelay = atoi((char*)rxBuffer);
		  if(tmpDelay > 0xFFFF) {
			  ledDelay = 0xFFFF;
		  } else {
			  ledDelay = tmpDelay;
		  }
		  sprintf(txBuffer, "Changed LED delay to: %u\r\nEnter LED Delay (0 - 65535):\r\n", ledDelay);
		  HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, strlen(txBuffer), UART_MAX_DELAY);
		  // Reset all memory for new input
		  memset(rxBuffer, 0, UART_RX_BUFFER_SIZE);
		  rxIndex = 0;
		  tmpDelay = 0;
		  // Resume blinking Thread, effectively restarting it, so it immediately picks up the new delay.
		  osThreadResume(blinkLED_taskHandle);
	  }
	  if(isdigit(rxByte)) {
		  // Ignore all characters that are not digits.
		  rxBuffer[rxIndex] = rxByte;
		  rxIndex++;
		  if(rxIndex == UART_RX_BUFFER_SIZE) {
			  sprintf(txBuffer, "Too many Digits!\r\nRe-enter number (0 - 65535):\r\n");
			  HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, strlen(txBuffer), UART_MAX_DELAY);
			  memset(rxBuffer, 0, UART_RX_BUFFER_SIZE);
			  rxIndex = 0;
		  }
	  }
	}
  /* USER CODE END UARTThread */
}

/* USER CODE BEGIN Header_BlinkLED */
/**
* @brief Function implementing the blinkLED_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BlinkLED */
void BlinkLED(void *argument)
{
  /* USER CODE BEGIN BlinkLED */
  /* Infinite loop */
	uint16_t current_delay;
  for(;;)
  {
	  current_delay = ledDelay;
	  HAL_GPIO_TogglePin(Board_LED_GPIO_Port, Board_LED_Pin);
	  osDelay(current_delay);
  }
  /* USER CODE END BlinkLED */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

