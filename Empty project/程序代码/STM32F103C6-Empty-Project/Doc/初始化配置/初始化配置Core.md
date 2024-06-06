##### common.h

```c
#ifndef __COMMON__H
#define __COMMON__H

/* LIB ------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/*---- Core -------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "spi.h"

/*---- Bsp -------------------------*/
#include "../Bsp/bsp_debug.h"
#include "../Bsp/bsp_wifi.h"
#include "../Bsp/bsp_nrf.h"

#define 	debugTaskSize					128

extern TaskHandle_t debugTaskHandle;

#endif /*__COMMON__H*/

```

##### freertos.c

```c
/* Private variables ---------------------------------------------------------*/
osThreadId debugTaskHandle;

/* Private function prototypes -----------------------------------------------*/
void debugTaskFun(void const * argument);

//FreeRTOS initialization
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN RTOS_THREADS */
    osThreadDef(debugTask, debugTaskFun, osPriorityNormal, 0, debugTaskSize);
	debugTaskHandle = osThreadCreate(osThread(debugTask), NULL);
}

/* Private application code --------------------------------------------------*/
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
```

##### stm32xxxx_it.c

```c
/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	BaseType_t phpt;
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) != RESET)) {
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);  //清除空闲状态
		xTaskNotifyFromISR( debugTaskHandle, 1U<<DEBUG_PARSE_DATA, eSetBits, &phpt );//DMA发送完成通知
		portYIELD_FROM_ISR( phpt );
		
	} else if ( __HAL_UART_GET_FLAG(&huart2,UART_FLAG_TC) != RESET ) {
		xTaskNotifyFromISR( debugTaskHandle, 1U<<DEBUG_SEND_OK, eSetBits, &phpt );//DMA发送完成通知
		portYIELD_FROM_ISR( phpt );
	}
  /* USER CODE END USART2_IRQn 1 */
}
```

##### bsp_debug.c

```c
#include "../Bsp/bsp_debug.h"
#define ds debug_str
DEBUG_STR debug_str;

void debug_init( void ) {
	taskENTER_CRITICAL();
	ds.rxBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.txBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	//开启DMA接收 开启串口空闲中断
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_IDLE );//使能空闲中断
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_TC );//使能发送完毕中断
	HAL_UART_AbortReceive( &DEBUG_HUART );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff, DEBUG_BUFF_SIZE );//启动DMA接收
}

void debug_parse_data_fun( void ) {
	vTaskDelay( 50 );
	HAL_UART_AbortReceive( &DEBUG_HUART );
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &DEBUG_HDMA_USART_RX );
	
	if ( strstr( (char *)ds.rxBuff, "debug" ) != NULL ) {
		printf("debug\r\n");
	} 
	memset( ds.rxBuff, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff, DEBUG_BUFF_SIZE );//启动DMA接收
}

```

##### bsp_debug.h

```c
#ifndef __BSP_DEBUG__H
#define __BSP_DEBUG__H

#include "../common.h"

#define DEBUG_HUART			huart2
#define DEBUG_HDMA_USART_RX	hdma_usart2_rx
#define DEBUG_HDMA_USART_TX	hdma_usart2_tx
#define DEBUG_BUFF_SIZE		128

/*---- debug task bits start ----------------*/
#define DEBUG_DEVICE_INIT 0
#define DEBUG_PARSE_DATA 	1
#define DEBUG_SEND_OK 		2
/*---- debug task bits end ----------------*/

typedef struct _DEBUG_STR {
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint32_t len;
} DEBUG_STR;

extern DEBUG_STR debug_str;

void debug_init( void );
void debug_parse_data_fun( void );

#endif /*__BSP_DEBUG__H*/

```

