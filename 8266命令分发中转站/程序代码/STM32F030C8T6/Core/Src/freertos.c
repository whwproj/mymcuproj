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
osThreadId debugTaskHandle;
TaskHandle_t wifi_control_taskHandle;
TaskHandle_t nrf_control_taskHandle;
TaskHandle_t data_task_handle;
TaskHandle_t time_task_handle;

QueueHandle_t wifi_data_handle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void debugTaskFun(void const * argument);
void wifi_control_task_fun(void const * argument);
void nrf_control_task_fun(void const * argument);
//void data_task_handle_fun(void const * argument);
void time_task_fun(void const * argument);
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
	wifi_data_handle = xQueueCreate( 3, 100 );
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	/*osThreadDef(debugTask, debugTaskFun, osPriorityNormal, 0, debugTaskSize);
  debugTaskHandle = osThreadCreate(osThread(debugTask), NULL);*/
	
	osThreadDef(wifi_control_task, wifi_control_task_fun, osPriorityNormal, 0, wifi_control_taskSize);
  wifi_control_taskHandle = osThreadCreate(osThread(wifi_control_task), NULL);
	
	osThreadDef(nrf_control_task, nrf_control_task_fun, osPriorityNormal, 0, nrf_control_taskSize);
  nrf_control_taskHandle = osThreadCreate(osThread(nrf_control_task), NULL);
	
//	osThreadDef(data_task, data_task_handle_fun, osPriorityNormal, 0, data_taskSize);
//  data_task_handle = osThreadCreate(osThread(data_task), NULL);
	
	osThreadDef(time_task, time_task_fun, osPriorityNormal, 0, time_taskSize);
  time_task_handle = osThreadCreate(osThread(time_task), NULL);

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
		led_init();
		//debug_init();
		
		HAL_TIM_Base_Start_IT( &htim3 );
		HAL_TIM_Base_Start( &htim3 );
		led_nrf_flicker_on();
		led_con_flicker_on();
	
		//xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_DEVICE_RESET, eSetBits );
		//vTaskDelay(1200);
		//xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STATION_MODE_INIT, eSetBits );
		//vTaskDelay(1000);
		//xTaskNotify( nrf_control_taskHandle, 1U<<NRF_INIT_EVENT, eSetBits );
		
#endif
		printf("init ok\r\n");
		vTaskDelete( defaultTaskHandle );
  }
	
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*--------------- DEBUG ----------------*/
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
		if ( oldBits & (1U<<DEVICE_NOT_REGISTER) ) {
			oldBits &=~ (1U<<DEVICE_NOT_REGISTER);
			send_device_not_register();
		}
		if ( oldBits & (1U<<DEVICE_NOT_ONLINE) ) {
			oldBits &=~ (1U<<DEVICE_NOT_ONLINE);
			send_device_not_online();
		}
		if ( oldBits & (1U<<FORWARD_SUCCESS) ) {
			oldBits &=~ (1U<<FORWARD_SUCCESS);
			send_forward_success();
		}
		if ( oldBits & (1U<<PARSE_WIFI_DATA) ) {
			oldBits &=~ (1U<<PARSE_WIFI_DATA);
			if ( parse_wifi_data_fun() != 0 ) {
				xTaskNotify( wifi_control_taskHandle, 1U<<PARSE_WIFI_DATA, eSetBits );
			}
		}
		
		/*if ( oldBits & (1U<<PUSH_DATA) ) {
			oldBits &=~ (1U<<PUSH_DATA);
			//push_data_fun();
		}*/
		
//		if ( oldBits & (1U<<WIFI_CONNECT_TCP0_) ) {
//			oldBits &=~ (1U<<WIFI_CONNECT_TCP0_);
//			//esp_connect_tcp0();
//		}
//		if ( oldBits & (1U<<WIFI_CONNECT_TCP1_) ) {
//			oldBits &=~ (1U<<WIFI_CONNECT_TCP1_);
//			//esp_connect_tcp1();
//		}
  }
}
void data_task_handle_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<PARSE_WIFI_DATA) ) {
			oldBits &=~ (1U<<PARSE_WIFI_DATA);
			if ( parse_wifi_data_fun() != 0 ) {
				xTaskNotify( data_task_handle, 1U<<PARSE_WIFI_DATA, eSetBits );
			}
		}
		/*printf("\r\n--- all / min \r\n");
		if ( wifi_control_taskHandle != NULL ) printf("%d / %ld   wifi_control\r\n", wifi_control_taskSize, uxTaskGetStackHighWaterMark(wifi_control_taskHandle) );
		printf("内存剩余%d Byte 历史小剩%d Byte\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
		printf("------ end ------\r\n");*/
		
  }
}
/*--------------- WIFI ----------------*/

/*--------------- NRF24 ----------------*/
void nrf_control_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		//xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, 5000 );
		oldBits |= newBits;
		if ( oldBits & (1U<<NRF_INIT_EVENT) ) {
			oldBits &=~ (1U<<NRF_INIT_EVENT);
			nrf_init();
		}
		if ( oldBits & (1U<<NRF_RX_EVENT) ) {
			oldBits &=~ (1U<<NRF_RX_EVENT);
		  nrf_receive_data();
		}
		if ( oldBits & (1U<<NRF_TX_EVENT) ) {
			oldBits &=~ (1U<<NRF_TX_EVENT);
			nrf_send_data();
		}
		printf("\r\n------ 内存剩余：%d Byte 历史最小内存剩余：%d Byte\r\n\r\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
  }
}
/*--------------- NRF24 ----------------*/

/*--------------- TIME ----------------*/
void time_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		xTaskNotifyWait( pdFALSE, pdFALSE, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<NRF_SEND_TIME) ) {
			oldBits &=~ (1U<<NRF_SEND_TIME);
			nrf_send_time_fun();
		}
	}
}
	
/*--------------- TIME ----------------*/

/* USER CODE END Application */

