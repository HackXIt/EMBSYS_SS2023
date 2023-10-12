/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for task_blink_r */
osThreadId_t task_blink_rHandle;
const osThreadAttr_t task_blink_r_attributes = {
  .name = "task_blink_r",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task_blink_g */
osThreadId_t task_blink_gHandle;
const osThreadAttr_t task_blink_g_attributes = {
  .name = "task_blink_g",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for task_blink_b */
osThreadId_t task_blink_bHandle;
const osThreadAttr_t task_blink_b_attributes = {
  .name = "task_blink_b",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal2,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void blink_red(void *argument);
void blink_green(void *argument);
void blink_blue(void *argument);

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
  /* creation of task_blink_r */
  task_blink_rHandle = osThreadNew(blink_red, NULL, &task_blink_r_attributes);

  /* creation of task_blink_g */
  task_blink_gHandle = osThreadNew(blink_green, NULL, &task_blink_g_attributes);

  /* creation of task_blink_b */
  task_blink_bHandle = osThreadNew(blink_blue, NULL, &task_blink_b_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_blink_red */
/**
  * @brief  Function implementing the task_blink_r thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_blink_red */
void blink_red(void *argument)
{
  /* USER CODE BEGIN blink_red */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END blink_red */
}

/* USER CODE BEGIN Header_blink_green */
/**
* @brief Function implementing the task_blink_g thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_blink_green */
void blink_green(void *argument)
{
  /* USER CODE BEGIN blink_green */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END blink_green */
}

/* USER CODE BEGIN Header_blink_blue */
/**
* @brief Function implementing the task_blink_b thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_blink_blue */
void blink_blue(void *argument)
{
  /* USER CODE BEGIN blink_blue */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END blink_blue */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

