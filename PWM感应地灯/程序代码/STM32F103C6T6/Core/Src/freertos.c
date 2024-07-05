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
#include "../Bsp/common.h"
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
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

QueueHandle_t cmd_queueHandle;
//QueueHandle_t send_The_Right_Mutex;

osThreadId usart_wifi_TaskHandle;
osThreadId usart_debug_TaskHandle;
osThreadId led_taskHandle;
osThreadId sleep_taskHandle;
osThreadId cmd_handle_taskHandle;
osThreadId pwm_taskHandle;
osThreadId check_online_taskHandle;


void usart_wifi_TaskHandleFun(void const *argument);
void usart_debug_TaskHandleFun(void const *argument);
void led_taskFun(void const *argument);
void sleep_taskFun(void const *argument);
void cmd_handle_taskFun(void const *argument);
void pwm_taskFun(void const *argument);
void check_online_taskFun(void const *argument);

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
//	send_The_Right_Mutex = xSemaphoreCreateMutex();
//	xSemaphoreGive( send_The_Right_Mutex );
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	cmd_queueHandle = xQueueCreate( 3, sizeof(SESSION*) );//������Ϣ����
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(usart_wifi_Task, usart_wifi_TaskHandleFun, osPriorityNormal, 0, 256);
	usart_wifi_TaskHandle = osThreadCreate(osThread(usart_wifi_Task), NULL);
	
	osThreadDef(cmd_handle_task, cmd_handle_taskFun, osPriorityNormal, 0, 128);
	cmd_handle_taskHandle = osThreadCreate(osThread(cmd_handle_task), NULL);
	
	osThreadDef(usart_debug_Task, usart_debug_TaskHandleFun, osPriorityNormal, 0, 128);
	usart_debug_TaskHandle = osThreadCreate(osThread(usart_debug_Task), NULL);
	
	osThreadDef(led_task, led_taskFun, osPriorityNormal, 0, 128);
	led_taskHandle = osThreadCreate(osThread(led_task), NULL);
	
	osThreadDef(sleep_task, sleep_taskFun, osPriorityNormal, 0, 128);
	sleep_taskHandle = osThreadCreate(osThread(sleep_task), NULL);
	
	osThreadDef(pwm_task, pwm_taskFun, osPriorityNormal, 0, 128);
	pwm_taskHandle = osThreadCreate(osThread(pwm_task), NULL);
	
	osThreadDef(check_online_task, check_online_taskFun, osPriorityNormal, 0, 128);
	check_online_taskHandle = osThreadCreate(osThread(check_online_task), NULL);
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
		//������ʼ��
		pwm_str.maxBtnPer = 100;
		
		//PWM
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);   //������ʱ��PWM���
		
//		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 1000 );//�޸�ռ�ձ�
//		while(1);
		
		//debug��ʼ��
		xTaskNotify( usart_debug_TaskHandle, 1U<<DEBUG_DEVICE_INIT, eSetBits );
		//wifi��ʼ��
		xTaskNotify( usart_wifi_TaskHandle, 1U<<WIFI_DEVICE_INIT, eSetBits );
		
		
		vTaskDelete( defaultTaskHandle );
		
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void usart_wifi_TaskHandleFun(void const * argument) {
	uint32_t newBits, oldBits;
	xTaskNotify( usart_wifi_TaskHandle, (1U<<WIFI_SEND_OK), eSetBits );
  for( ;; ) {
		xTaskNotifyWait( pdFALSE, ~(1U<<WIFI_SEND_OK), &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<WIFI_DEVICE_INIT) ) {//��ʼ��wifi����
			oldBits &=~ (1U<<WIFI_DEVICE_INIT);
			wifi_init();
		}
		if ( oldBits & (1U<<WIFI_PARSE_DATA) ) {//��������������
			oldBits &=~ (1U<<WIFI_PARSE_DATA);
			wifi_parse_data();
		}
		if ( oldBits & (1U<<WIFI_CHECK_ONLINE) ) {//xs����ͨ��,���wifi����״̬
			oldBits &=~ (1U<<WIFI_CHECK_ONLINE);
			wifi_str.isConfig = 1;
			if ( wifi_check_online() == pdFALSE) {
				wifi_to_reconfigure();
			}
			wifi_str.isConfig = 0;
		}
		
  }
}

void usart_debug_TaskHandleFun(void const * argument) {
	uint32_t newBits, oldBits;
  for( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<DEBUG_DEVICE_INIT) ) {//��ʼ��wifi����
			oldBits &=~ (1U<<DEBUG_DEVICE_INIT);
			debug_init();
		}
		if ( oldBits & (1U<<DEBUG_PARSE_DATA) ) {//��������������
			oldBits &=~ (1U<<DEBUG_PARSE_DATA);
			debug_parse_data();
		}
  }
}
void led_taskFun(void const * argument) {
	uint32_t newBits, oldBits;
  for( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<LED_FAST_FLASHING) ) {
			oldBits &=~ (1U<<LED_FAST_FLASHING);
			for ( ;; ) {
				xTaskNotify( led_taskHandle, pdFALSE, eSetBits );
				xTaskNotifyWait( pdFALSE, 0, &newBits, 0 );
				if ( newBits ) {
					xTaskNotify( led_taskHandle, newBits, eSetBits );
					break;
				}
				vTaskDelay(150);
				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			}
		}
		if ( oldBits & (1U<<LED_SLOW_FLASHING) ) {
			oldBits &=~ (1U<<LED_SLOW_FLASHING);
			for ( ;; ) {
				xTaskNotify( led_taskHandle, pdFALSE, eSetBits );
				xTaskNotifyWait( pdFALSE, 0, &newBits, 0 );
				if ( newBits ) {
					xTaskNotify( led_taskHandle, newBits, eSetBits );
					break;
				}
				vTaskDelay(800);
				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			}
		}
		if ( oldBits & (1U<<LED_TURN_OFF) ) {
			oldBits &=~ (1U<<LED_TURN_OFF);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		}
		if ( oldBits & (1U<<LED_TURN_ON) ) {
			oldBits &=~ (1U<<LED_TURN_ON);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		}
  }
}

void sleep_taskFun(void const * argument) {
	uint32_t newBits, oldBits;
  for( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<FALL_SLEEP) ) {//��ʱ����˯��
			oldBits &=~ (1U<<FALL_SLEEP);
			
		}
		if ( oldBits & (1U<<WAKE_UP) ) {//��ʱ����
			oldBits &=~ (1U<<WAKE_UP);
			
		}
  }
}

void cmd_handle_taskFun(void const * argument) {
	uint32_t newBits, oldBits;
	xTaskNotify( usart_wifi_TaskHandle, (1U<<WIFI_SEND_OK), eSetBits );
  for( ;; ) {
		xTaskNotifyWait( pdFALSE, ~(1U<<WIFI_SEND_OK), &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<CMD_MAIN) ) {//���˵�
			oldBits &=~ (1U<<CMD_MAIN);
			if ( cmd_main_fun() != 0 ) {
				xTaskNotify( cmd_handle_taskHandle, 1U<<CMD_MAIN, eSetBits );
			}
		}
		if ( oldBits & (1U<<CMD_SUB_1) ) {
			oldBits &=~ (1U<<CMD_SUB_1);
			if ( cmd_sub_1_fun() != 0 ) {
				xTaskNotify( cmd_handle_taskHandle, 1U<<CMD_SUB_1, eSetBits );
			}
		}
		if ( oldBits & (1U<<CMD_SUB_2) ) {
			oldBits &=~ (1U<<CMD_SUB_2);
			if ( cmd_sub_2_fun() != 0 ) {
				xTaskNotify( cmd_handle_taskHandle, 1U<<CMD_SUB_2, eSetBits );
			}
		}
		if ( oldBits & (1U<<CMD_SUB_3) ) {
			oldBits &=~ (1U<<CMD_SUB_3);
			if ( cmd_sub_3_fun() != 0 ) {
				xTaskNotify( cmd_handle_taskHandle, 1U<<CMD_SUB_3, eSetBits );
			}
		}
  }
}

void pwm_taskFun(void const * argument) {
	uint32_t newBits, oldBits;
  for( ;; ) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<TURN_ON) ) {
			oldBits &=~ (1U<<TURN_ON);
			pwm_turn_on();
		}
		if ( oldBits & (1U<<TURN_OFF) ) {
			oldBits &=~ (1U<<TURN_OFF);
			pwm_turn_off();
		}
		if ( oldBits & (1U<<FLASHING) ) {
			oldBits &=~ (1U<<FLASHING);
			pwm_flashing();
		}
  }
}

void check_online_taskFun(void const * argument) {
	uint32_t newBits, oldBits;
	uint8_t num=0,i;
	vTaskDelay(1000);
  for( ;; ) {
		vTaskDelay(1000);
		if ( wifi_str.isConfig == 0 ) {
			if ( num++ > 5 ) {
				num = 0;
				xTaskNotify( check_online_taskHandle, pdFALSE, eSetBits );
				xTaskNotifyWait( pdFALSE, 1U<<WIFI_IS_COMMUNICATION, &newBits, portMAX_DELAY );
				if ( !(newBits & (1U<<WIFI_IS_COMMUNICATION)) ) {
					xTaskNotify( usart_wifi_TaskHandle, 1U<<WIFI_CHECK_ONLINE, eSetBits );
				} else {
					wifi_str.checkOnlineNum = 0;
				}
			}
		} else {
			num = 0;
		}
		for( i=0; i<2; i++ ) {
			if ( session[i].onlineSta ) {
				if ( session[i].heart > 0 ) {
					session[i].heart--;
				} else {
					xTaskNotify( check_online_taskHandle, pdFALSE, eSetBits );
					xTaskNotifyWait( pdFALSE, 1U<<NO_SESSION, &newBits, portMAX_DELAY );
					if ( !(newBits & (1U<<NO_SESSION)) ) {
						session[i].onlineSta = 0;
						session[i].dir = 0;
					}
				}
			}
		}
  }
}
/* USER CODE END Application */

