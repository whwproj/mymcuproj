/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
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
#include "../common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
STR str;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
TaskHandle_t nrf_control_taskHandle;
TaskHandle_t executive_taskHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
#ifdef DEBUG_ENABLE
void debugTaskFun(void const * argument);
#endif
void executive_task_fun(void const * argument);
void nrf_control_task_fun(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

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
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
#ifdef DEBUG_ENABLE
	osThreadDef(debugTask, debugTaskFun, osPriorityNormal, 0, debugTaskSize);
  debugTaskHandle = osThreadCreate(osThread(debugTask), NULL);
#endif
	
	osThreadDef(nrf_control_task, nrf_control_task_fun, osPriorityNormal, 0, nrf_control_taskSize);
  nrf_control_taskHandle = osThreadCreate(osThread(nrf_control_task), NULL);
	
	osThreadDef(executive_task, executive_task_fun, osPriorityNormal, 0, executive_taskSize);
  executive_taskHandle = osThreadCreate(osThread(executive_task), NULL);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
//		LED_ALL_OFF();
//		//LED0_ON();
//		//getSnByDeviceId_setClientId();
//		//read_data_from_flash();
//		//#ifdef DEBUG_ENABLE
//		//	debug_init();
//		//#endif
//		HAL_TIM_Base_Start_IT( &htim3 );
//		HAL_TIM_Base_Start( &htim3 );
//		
//		xTaskNotify( nrf_control_taskHandle, 1U<<NRF_INIT_EVENT, eSetBits );

//		
//		vTaskDelay(100);
//		//LED0_OFF();
////		xTaskNotify( nrf_control_taskHandle, 1U<<NRF_REGISTER_DEVICE, eSetBits );
		LED_ALL_OFF();
		nrf_init();
		vTaskDelay(100);
		printf("init ok\r\n");
		
		for ( ;; ) {
			vTaskDelay(300);
			nrf_send_test();
		}
		
		vTaskDelete( defaultTaskHandle );
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/*--------------- LED ----------------*/
void executive_task_fun(void const * argument) {
	uint32_t newBits, oldBits;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<LED_MODE_0) ) {
			oldBits &=~ (1U<<LED_MODE_0);
			led_mode_0_fun();
		}
		if ( oldBits & (1U<<LED_MODE_OFF) ) {
			oldBits &=~ (1U<<LED_MODE_OFF);
			led_mode_off_fun();
		}
  }
}
/*--------------- LED ----------------*/

/*--------------- NRF24 ----------------*/
void nrf_control_task_fun(void const * argument) {
	uint32_t newBits, oldBits;
	
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<NRF_INIT_EVENT) ) {
			oldBits &=~ (1U<<NRF_INIT_EVENT);
			nrf_init();
		}
		if ( oldBits & (1U<<NRF_SEND_DATA) ) {
			oldBits &=~ (1U<<NRF_SEND_DATA);
			nrf_send_data();
		}
		if ( oldBits & (1U<<NRF_PARSE_DATA) ) {
			oldBits &=~ (1U<<NRF_PARSE_DATA);
			nrf_parse_data();
		}
		if ( oldBits & (1U<<NRF_REGISTER_DEVICE) ) {
			oldBits &=~ (1U<<NRF_REGISTER_DEVICE);
			nrf_register_device();
		}
  }
}
/*--------------- NRF24 ----------------*/


/* USER CODE END Application */

