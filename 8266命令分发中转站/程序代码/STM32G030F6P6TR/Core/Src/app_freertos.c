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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId debugTaskHandle;
TaskHandle_t wifi_control_taskHandle;
TaskHandle_t nrf_control_taskHandle;

TaskHandle_t wifi_tcp_connect_taskHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
#ifdef DEBUG_ENABLE
void debugTaskFun(void const * argument);
#endif
void wifi_control_task_fun(void const * argument);
void wifi_tcp_connect_task_fun(void const * argument);
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
	osThreadDef(wifi_control_task, wifi_control_task_fun, osPriorityNormal, 0, wifi_control_taskSize);
  wifi_control_taskHandle = osThreadCreate(osThread(wifi_control_task), NULL);
	
	osThreadDef(nrf_control_task, nrf_control_task_fun, osPriorityNormal, 0, nrf_control_taskSize);
  nrf_control_taskHandle = osThreadCreate(osThread(nrf_control_task), NULL);
	
	osThreadDef(wifi_tcp_connect_task, wifi_tcp_connect_task_fun, osPriorityNormal, 0, wifi_tcp_connect_taskSize);
  wifi_tcp_connect_taskHandle = osThreadCreate(osThread(wifi_tcp_connect_task), NULL);
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
#ifdef WIFIDEBUG//烧录固件
		HAL_GPIO_WritePin( ESP_EN_GPIO_Port, ESP_EN_Pin, GPIO_PIN_SET );
		HAL_GPIO_WritePin( ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET );
		vTaskDelay( 500 );
		HAL_GPIO_WritePin( ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_SET );
		vTaskDelay( 2000 );
#else
		read_data_from_flash();
		led_init();
	#ifdef DEBUG_ENABLE
		debug_init();
	#endif
		HAL_TIM_Base_Start_IT( &htim3 );
		HAL_TIM_Base_Start( &htim3 );
		led_nrf_flicker_on();
		led_con_flicker_on();

		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_DEVICE_RESET, eSetBits );
		vTaskDelay(1200);
		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STATION_MODE_INIT, eSetBits );
		vTaskDelay(1000);
		
		xTaskNotify( nrf_control_taskHandle, 1U<<NRF_INIT_EVENT, eSetBits );
		vTaskDelay(500);

#endif		
		printf("init ok\r\n");

		vTaskDelete( defaultTaskHandle );
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*--------------- DEBUG ----------------*/
#ifdef DEBUG_ENABLE
void debugTaskFun(void const * argument) {
	uint32_t newBits, oldBits = 0;
	xTaskNotify( debugTaskHandle, 1U<<DEBUG_SEND_OK, eSetBits );
  for(;;) {
		xTaskNotifyWait( pdFALSE, ~(1U<<DEBUG_SEND_OK), &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<DEBUG_PARSE_DATA) ) {
			oldBits &=~ (1U<<DEBUG_PARSE_DATA);
			debug_parse_data_fun();
		}
  }
}
#endif
/*--------------- DEBUG ----------------*/

/*--------------- WIFI ----------------*/
void wifi_control_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<WIFI_DEVICE_RESET) ) {
			oldBits &=~ (1U<<WIFI_DEVICE_RESET);
			wifi_reset();
		}
		if ( oldBits & (1U<<WIFI_UART_IDLE_CALLBACK) ) {
			oldBits &=~ (1U<<WIFI_UART_IDLE_CALLBACK);
			wifi_uart_idle_callback();
		}
		if ( oldBits & (1U<<WIFI_STATION_MODE_INIT) ) {
			oldBits &=~ (1U<<WIFI_STATION_MODE_INIT);
			station_mode_init();
		}
		if ( oldBits & (1U<<WIFI_STA_AP_MODE_INIT) ) {
			oldBits &=~ (1U<<WIFI_STA_AP_MODE_INIT);
			station_and_ap_init();
		}
		if ( oldBits & (1U<<WIFI_SEND_HEART) ) {
			oldBits &=~ (1U<<WIFI_SEND_HEART);
			send_mqtt_heart();
		}
  }
}
void wifi_tcp_connect_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		//xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		//xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, 10000 );
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, 800 );
		nrf_send_data();

		//printf("\r\n------ 单个任务堆栈的历史最小内存 总大小 / 历史最小内存 start ------\r\n");
//		printf("\r\n--- all / min \r\n");
//		if ( wifi_control_taskHandle != NULL ) printf("%d / %ld   wifi_control\r\n", wifi_control_taskSize, uxTaskGetStackHighWaterMark(wifi_control_taskHandle) );
//		if ( wifi_tcp_connect_taskHandle != NULL ) printf("%d / %ld  wifi_tcp_connect\r\n", wifi_tcp_connect_taskSize ,uxTaskGetStackHighWaterMark(wifi_tcp_connect_taskHandle));
//		printf("内存剩余：%d Byte 历史最小剩：%d Byte\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
//		printf("------ end ------\r\n");
		
  }
}
/*--------------- WIFI ----------------*/

/*--------------- NRF24 ----------------*/
void nrf_control_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		//xTaskNotifyWait( pdFALSE, ~(1U<<DEBUG_SEND_OK), &newBits, portMAX_DELAY );
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<NRF_INIT_EVENT) ) {
			oldBits &=~ (1U<<NRF_INIT_EVENT);
			nrf_init();
		}
		if ( oldBits & (1U<<NRF_TX_EVENT) ) {
			oldBits &=~ (1U<<NRF_TX_EVENT);
			Tx_Mode();
			vTaskDelay(100);
			nrf_send_data();
			vTaskDelay(100);
			//Rx_Mode();
			
		}
		if ( oldBits & (1U<<NRF_RX_EVENT) ) {
			oldBits &=~ (1U<<NRF_RX_EVENT);
			nrf_receive_data();
			//vTaskDelay(500);
			//xTaskNotify( nrf_control_taskHandle, 1U<<NRF_TX_EVENT, eSetBits );
		}
		
  }
}
/*--------------- NRF24 ----------------*/

/* USER CODE END Application */

