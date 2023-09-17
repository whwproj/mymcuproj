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
TaskHandle_t Debug_TaskHandle;
TaskHandle_t wifi_control_taskHandle;
TaskHandle_t wifi_tcp_connect_taskHandle;

QueueHandle_t TcpData_QueueHandle;//数据消息队列
QueueHandle_t Mqtt_Ask_And_Heartbeat_QueueHandle;//mqtt应答以及心跳消息队列

//QueueHandle_t communication_Mutex;//信号�?,�?通道多tcp,隔离回话信号�?
QueueHandle_t TcpRxCache_Mutex;//获取接收缓存空间信号�?
//QueueHandle_t TcpTxCache_Mutex;//获取发�?�缓存空间信号量
QueueHandle_t TxSend_Mutex;//发�?�数据信号量
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Debug_TaskHandleFun(void const * argument);
void wifi_control_task_fun(void const * argument);
void wifi_tcp_connect_task_fun(void const * argument);

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
	//communication_Mutex = xSemaphoreCreateMutex();//接收数据和AT指令抢占的信号量
	//xSemaphoreGive( communication_Mutex );
	TcpRxCache_Mutex = xSemaphoreCreateMutex();//获取发�?�缓存空间信号量
	xSemaphoreGive( TcpRxCache_Mutex );
	TxSend_Mutex = xSemaphoreCreateMutex();//发�?�数据信号量
	xSemaphoreGive( TxSend_Mutex );
	//TcpRxCache_QueueHandle = xSemaphoreCreateMutex();//获取接收缓存空间信号�?
	//xSemaphoreGive( TcpRxCache_QueueHandle );

  TcpData_QueueHandle = xQueueCreate( 2,  sizeof( TCP_DATA* ) );
	Mqtt_Ask_And_Heartbeat_QueueHandle = xQueueCreate( 6, 4 );//mqtt应答以及心跳消息队列
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	//Debug
	osThreadDef(Debug_Task, Debug_TaskHandleFun, osPriorityNormal, 0, Debug_TaskSize);
  Debug_TaskHandle = osThreadCreate(osThread(Debug_Task), NULL);
	//wifi Control
	osThreadDef(wifi_control_task, wifi_control_task_fun, osPriorityNormal, 0, wifi_control_taskSize);
  wifi_control_taskHandle = osThreadCreate(osThread(wifi_control_task), NULL);
	//wifi connect
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
		debug_init();
		printf("init");
		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_DEVICE_INIT, eSetBits );
		vTaskDelete( defaultTaskHandle );
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Debug_TaskHandleFun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<DEBUG_DEVICE_INIT) ) {
			oldBits &=~ (1U<<DEBUG_DEVICE_INIT);
			debug_init();
		}
    if ( oldBits & (1U<<DEBUG_PARSE_DATA) ) {
			oldBits &=~ (1U<<DEBUG_PARSE_DATA);
			debug_parse_data_fun();
		}
		if ( oldBits & (1U<<DEBUG_SEND_OK) ) {
			oldBits &=~ (1U<<DEBUG_SEND_OK);
		}
  }
}

/*--------------- WIFI ----------------*/
void wifi_control_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<WIFI_DEVICE_INIT) ) {
			oldBits &=~ (1U<<WIFI_DEVICE_INIT);
			wifi_init();
		}
		if ( oldBits & (1U<<WIFI_CONNECT_TCP0_) ) {
			oldBits &=~ (1U<<WIFI_CONNECT_TCP0_);
			esp_connect_tcp0();
		}
		if ( oldBits & (1U<<WIFI_CONNECT_TCP1_) ) {
			oldBits &=~ (1U<<WIFI_CONNECT_TCP1_);
			esp_connect_tcp1();
		}
    if ( oldBits & (1U<<WIFI_PARSE_DATA) ) {
			oldBits &=~ (1U<<WIFI_PARSE_DATA);
			wifi_mqtt_data_parse();
		}
		if ( oldBits & (1U<<WIFI_TCP0_SEND) ) {
			oldBits &=~ (1U<<WIFI_TCP0_SEND);
			wifi_tcp0_send_data();
		}
		if ( oldBits & (1U<<WIFI_SEND_HEART) ) {
			oldBits &=~ (1U<<WIFI_SEND_HEART);
			wifi_mqtt_heart();
		}
		if ( oldBits & (1U<<WIFI_SEND_OK) ) {
			oldBits &=~ (1U<<WIFI_SEND_OK);
		}
  }
}
void wifi_tcp_connect_task_fun(void const * argument) {
	uint32_t newBits, oldBits = 0;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<WIFI_CONNECT_TCP0_DELAY) ) {
			oldBits &=~ (1U<<WIFI_CONNECT_TCP0_DELAY);
			vTaskDelay(5000);
			esp_connect_tcp0();
		}
		if ( oldBits & (1U<<WIFI_CONNECT_TCP1_DELAY) ) {
			oldBits &=~ (1U<<WIFI_CONNECT_TCP1_DELAY);
			vTaskDelay(5000);
			esp_connect_tcp1();
		}
  }
}
/*--------------- WIFI ----------------*/

/* USER CODE END Application */

