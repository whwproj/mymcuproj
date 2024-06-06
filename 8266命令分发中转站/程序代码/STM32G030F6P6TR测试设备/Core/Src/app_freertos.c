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
TaskHandle_t executive_taskHandle;
TaskHandle_t nrf_control_taskHandle;
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
		getSnByDeviceId_setClientId();
		read_data_from_flash();
		led_init();
	#ifdef DEBUG_ENABLE
		debug_init();
	#endif
		HAL_TIM_Base_Start_IT( &htim3 );
		HAL_TIM_Base_Start( &htim3 );
//		led_nrf_flicker_on();
//		led_con_flicker_on();

//		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_DEVICE_RESET, eSetBits );
//		vTaskDelay(1200);
//		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STATION_MODE_INIT, eSetBits );
//		vTaskDelay(1000);
		
		xTaskNotify( nrf_control_taskHandle, 1U<<NRF_INIT_EVENT, eSetBits );
		vTaskDelay(500);
		
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

/*--------------- xxxxx ----------------*/
void executive_task_fun(void const * argument) {
	uint32_t newBits, oldBits;
  for(;;) {
		xTaskNotifyWait( pdFALSE, portMAX_DELAY, &newBits, portMAX_DELAY );
		oldBits |= newBits;
		if ( oldBits & (1U<<EXECUT_TEST) ) {
			oldBits &=~ (1U<<EXECUT_TEST);
			led_execut_test();
		}
  }
}
/*--------------- xxxxx ----------------*/

/*--------------- NRF24 ----------------*/
void nrf_control_task_fun(void const * argument) {
	uint32_t newBits, t_start, t_end, oldBits = 0;
	
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
			nrf_send_data();
		}
		if ( oldBits & (1U<<NRF_RX_EVENT) ) {
			oldBits &=~ (1U<<NRF_RX_EVENT);
			nrf_parse_data();
			
//			nrf1_receive_data();
//			vTaskDelay(500);
//			HAL_NVIC_DisableIRQ(EXTI4_IRQn);
//			Tx_Mode(&hspi1);
//			nrf1_send_data();
//			for ( i=10; i>0; i-- ) {
//				sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP, &hspi1 );
//				//已发送
//				if ( !(sta & 0x01) ) break;
//				vTaskDelay(1);
//			}
//			//无应答,清空缓存
//			if ( i == 0 ) { SPI_RW_Reg( FLUSH_RX,NOP, &hspi1 );}
//			
//			Rx_Mode(&hspi1);
//			sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP, &hspi1 );
//			SPI_RW_Reg( NRF_WRITE_REG + STATUS, sta, &hspi1 );
//			__HAL_GPIO_EXTI_CLEAR_IT(SPI1_IRQ_Pin);
//			HAL_NVIC_EnableIRQ(EXTI4_IRQn);
		}
		if ( oldBits & (1U<<NRF_PARSE_DATA) ) {
			oldBits &=~ (1U<<NRF_PARSE_DATA);
			nrf_parse_data();
		}
		
  }
}
/*--------------- NRF24 ----------------*/

/* USER CODE END Application */

