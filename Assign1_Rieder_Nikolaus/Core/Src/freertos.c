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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// Source: https://mcuoneclipse.com/2018/05/21/understanding-freertos-task-stack-usage-and-kernel-awarness-information/
#define configRECORD_STACK_HIGH_ADDRESS (1)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 64 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for allocTask1 */
osThreadId_t allocTask1Handle;
const osThreadAttr_t allocTask1_attributes = {
  .name = "allocTask1",
  .stack_size = 64 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for allocTask2 */
osThreadId_t allocTask2Handle;
const osThreadAttr_t allocTask2_attributes = {
  .name = "allocTask2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for allocTask3 */
osThreadId_t allocTask3Handle;
const osThreadAttr_t allocTask3_attributes = {
  .name = "allocTask3",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for pollStackSize */
osThreadId_t pollStackSizeHandle;
uint32_t pollStackSizeBuffer[ 512 ];
osStaticThreadDef_t pollStackSizeControlBlock;
const osThreadAttr_t pollStackSize_attributes = {
  .name = "pollStackSize",
  .cb_mem = &pollStackSizeControlBlock,
  .cb_size = sizeof(pollStackSizeControlBlock),
  .stack_mem = &pollStackSizeBuffer[0],
  .stack_size = sizeof(pollStackSizeBuffer),
  .priority = (osPriority_t) osPriorityLow2,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void DefaultTask(void *argument);
extern void AllocTask1(void *argument);
extern void AllocTask2(void *argument);
extern void AllocTask3(void *argument);
extern void PollStackSize(void *argument);

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(DefaultTask, NULL, &defaultTask_attributes);

  /* creation of allocTask1 */
  allocTask1Handle = osThreadNew(AllocTask1, NULL, &allocTask1_attributes);

  /* creation of allocTask2 */
  allocTask2Handle = osThreadNew(AllocTask2, NULL, &allocTask2_attributes);

  /* creation of allocTask3 */
  allocTask3Handle = osThreadNew(AllocTask3, NULL, &allocTask3_attributes);

  /* creation of pollStackSize */
  pollStackSizeHandle = osThreadNew(PollStackSize, NULL, &pollStackSize_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_DefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_DefaultTask */
void DefaultTask(void *argument)
{
  /* USER CODE BEGIN DefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END DefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

