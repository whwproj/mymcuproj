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
TaskHandle_t wifi_control_taskHandle;
TaskHandle_t nrf_control_taskHandle;
TaskHandle_t wifi_send_taskHandle;

QueueHandle_t wifi_send_cache_handle;
QueueHandle_t wifi_receive_cache_handle;
QueueHandle_t wifi_using_lock_handle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void debugTaskFun(void const * argument);
void wifi_control_task_fun(void const * argument);
void wifi_send_task_fun(void const * argument);
void nrf_control_task_fun(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
	wifi_send_cache_handle = xSemaphoreCreateCounting( 3, 3 );
	wifi_receive_cache_handle = xSemaphoreCreateCounting( 3, 3 );
	wifi_using_lock_handle = xSemaphoreCreateMutex();
	xSemaphoreGive(wifi_using_lock_handle);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(wifi_control_task, wifi_control_task_fun, osPriorityAboveNormal, 0, wifi_control_taskSize);
  wifi_control_taskHandle = osThreadCreate(osThread(wifi_control_task), NULL);
	
	osThreadDef(nrf_control_task, nrf_control_task_fun, osPriorityNormal, 0, nrf_control_taskSize);
  nrf_control_taskHandle = osThreadCreate(osThread(nrf_control_task), NULL);

	osThreadDef(wifi_send_task, wifi_send_task_fun, osPriorityNormal, 0, wifi_send_taskSize);
  wifi_send_taskHandle = osThreadCreate(osThread(wifi_send_task), NULL);

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
#ifdef WIFIDEBUG
		//复位ESP
		HAL_GPIO_WritePin( ESP_EN_GPIO_Port, ESP_EN_Pin, GPIO_PIN_SET );
		HAL_GPIO_WritePin( ESP_IO0_GPIO_Port, ESP_IO0_Pin, GPIO_PIN_RESET );
		HAL_GPIO_WritePin( ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET );
		vTaskDelay( 500 );
		HAL_GPIO_WritePin( ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_SET );
		vTaskDelay( 2000 );
#else
		read_data_from_w25qFlash();
		getSnByDeviceId_setClientId();
		led_init();
		HAL_TIM_Base_Start_IT( &htim3 );
		HAL_TIM_Base_Start( &htim3 );
		led_nrf_flicker_on();
		led_con_flicker_on();
		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_DEVICE_RESET, eSetBits );
		vTaskDelay(1200);
		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STATION_MODE_INIT, eSetBits );
		//xTaskNotify( nrf_control_taskHandle, 1U<<NRF_INIT_EVENT, eSetBits );
#endif		
		printf("init ok\r\n");
		vTaskDelete( defaultTaskHandle );
  }
	
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

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
		if ( oldBits & (1U<<PARSE_JSON_DATA) ) {
			oldBits &=~ (1U<<PARSE_JSON_DATA);
			parse_wifi_data_fun();
		}
  }
}

void wifi_send_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		//xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, 3000 );
		oldBits |= newBits;
		if ( oldBits & (1U<<WIFI_SEND_HEART) ) {
			oldBits &=~ (1U<<WIFI_SEND_HEART);
			send_mqtt_heart();
		}
		if ( oldBits & (1U<<WIFI_SEND_MQTT) ) {
			oldBits &=~ (1U<<WIFI_SEND_MQTT);
			wifi_send_mqtt();
		}
		
		//printf("\r\n------ 总大�? / 历史�?小内�? start ------\r\n");
		//portENTER_CRITICAL();
//		printf("\r\n------ start ------\r\n");
//		if ( wifi_control_taskHandle != NULL ) printf("%d / %ld   wifi_control_taskHandle\r\n", wifi_control_taskSize, uxTaskGetStackHighWaterMark(wifi_control_taskHandle) );
//		if ( nrf_control_taskHandle != NULL ) printf("%d / %ld   nrf_control_taskHandle\r\n", nrf_control_taskSize, uxTaskGetStackHighWaterMark(nrf_control_taskHandle) );
//		if ( wifi_send_taskHandle != NULL ) printf("%d / %ld   wifi_send_taskHandle\r\n", wifi_send_taskSize, uxTaskGetStackHighWaterMark(wifi_send_taskHandle) );
//		printf("内存剩余�?%d Byte 历史�?小内存剩余：%d Byte\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
//		printf("\r\n------ end ------\r\n");
		//portEXIT_CRITICAL();
		//printf("------ 单个任务堆栈的历史最小内�? end ------\r\n");
  }
}

//void data_task_handle_fun(void const * argument) {
//	uint32_t newBits, oldBits = 0;
//  for(;;) {
//		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
//		oldBits |= newBits;
//		if ( oldBits & (1U<<PARSE_WIFI_DATA) ) {
//			oldBits &=~ (1U<<PARSE_WIFI_DATA);
//			if ( parse_wifi_data_fun() != 0 ) {
//				xTaskNotify( data_task_handle, 1U<<PARSE_WIFI_DATA, eSetBits );
//			}
//		}
//		/*printf("\r\n--- all / min \r\n");
//		if ( wifi_control_taskHandle != NULL ) printf("%d / %ld   wifi_control\r\n", wifi_control_taskSize, uxTaskGetStackHighWaterMark(wifi_control_taskHandle) );
//		printf("内存剩余%d Byte 历史小剩%d Byte\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
//		printf("------ end ------\r\n");*/
//		
//  }
//}
/*--------------- WIFI ----------------*/

/*--------------- NRF24 ----------------*/
void nrf_control_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<NRF_INIT_EVENT) ) {
			oldBits &=~ (1U<<NRF_INIT_EVENT);
			nrf_init();
			//wifi_msg_add_SendList( 15, 10, "test data测试数据", 0 );
		}
		if ( oldBits & (1U<<NRF_RX_EVENT) ) {
			oldBits &=~ (1U<<NRF_RX_EVENT);
			LED_NRF_Toggle();
		  nrf_receive_data();
		}
		if ( oldBits & (1U<<TIM_CLEAR_NRFREG) ) {
			oldBits &=~ (1U<<TIM_CLEAR_NRFREG);
			tim_clear_nrfreg();
		}
		
		// if ( oldBits & (1U<<NRF_TX_EVENT) ) {
		// 	oldBits &=~ (1U<<NRF_TX_EVENT);
		// 	nrf_send_data();
		// }

//		printf("\r\n------ 总大�? / 历史�?小内�? start ------\r\n");
//		if ( wifi_control_taskHandle != NULL ) printf("%d / %ld   wifi_control_taskHandle\r\n", wifi_control_taskSize, uxTaskGetStackHighWaterMark(wifi_control_taskHandle) );
//		if ( nrf_control_taskHandle != NULL ) printf("%d / %ld   nrf_control_taskHandle\r\n", nrf_control_taskSize, uxTaskGetStackHighWaterMark(nrf_control_taskHandle) );
//		if ( wifi_send_taskHandle != NULL ) printf("%d / %ld   wifi_send_taskHandle\r\n", wifi_send_taskSize, uxTaskGetStackHighWaterMark(wifi_send_taskHandle) );
//		printf("内存剩余�?%d Byte 历史�?小内存剩余：%d Byte\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
//		printf("------ 单个任务堆栈的历史最小内�? end ------\r\n");

  }
}
/*--------------- NRF24 ----------------*/


/* USER CODE END Application */

