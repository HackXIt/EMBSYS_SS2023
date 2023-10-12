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
#include <string.h>
#include "myTasks.h"
#include "ringbuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/*
 * Create NUM_PRODS producer tasks that add some data using Ringbuffer_Put() periodically.
 * NUM_PRODS shall be a compile-time constant that can be set to
 * a value between 1 and (at least) 10
 * without changing anything else in the code or IOC file.
 */
#define NUM_PRODS			10
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Definitions for the application */
osMemoryPoolId_t taskPoolHandle;
uint8_t producerCount = NUM_PRODS;
/* USER CODE END Variables */
/* Definitions for consumerTask */
osThreadId_t consumerTaskHandle;
const osThreadAttr_t consumerTask_attributes = {
  .name = "consumerTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for InitTask */
osThreadId_t InitTaskHandle;
const osThreadAttr_t InitTask_attributes = {
  .name = "InitTask",
  .stack_size = 64 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void ConsumerTask(void *argument);
void StartTasks(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	taskPoolHandle = Ringbuffer_Init(NUM_PRODS);

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
  /* creation of consumerTask */
  consumerTaskHandle = osThreadNew(ConsumerTask, NULL, &consumerTask_attributes);

  /* creation of InitTask */
  InitTaskHandle = osThreadNew(StartTasks, (void*) &producerCount, &InitTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_ConsumerTask */
/**
  * @brief  Function implementing the consumerTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ConsumerTask */
__weak void ConsumerTask(void *argument)
{
  /* USER CODE BEGIN ConsumerTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ConsumerTask */
}

/* USER CODE BEGIN Header_StartTasks */
/**
* @brief Function implementing the InitTasks thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTasks */
__weak void StartTasks(void *argument)
{
  /* USER CODE BEGIN StartTasks */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTasks */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

