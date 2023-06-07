#include "bsp_debug.h"

#define ds debug_str

DEBUG_STR debug_str;

void debug_init( void ) {
	taskENTER_CRITICAL();
	ds.rxBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.txBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	//开启DMA接收 开启串口空闲中断
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_IDLE );//使能空闲中断
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_TC );//使能发送完毕中断
	HAL_UART_AbortReceive( &huart1 );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );//启动DMA接收
}


void debug_parse_data( void ) {
	vTaskDelay( 50 );
	HAL_UART_AbortReceive( &huart1 );
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	//判断类型
	if ( buffCompareToBuff( "连接4G中", (char *)ds.rxBuff, strlen("连接4G中") ) ) {
		//xTaskNotify( TJC_ControlTaskHandle, 1U<<TJC_CON_WH_LTE_DISCON_TCP232, eSetBits );//连接4G中
	} else {
		HAL_UART_Transmit( &huart2, ds.rxBuff, ds.len, 0xffff );
	}
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );//启动DMA接收
}

BaseType_t buffCompareToBuff( char *buff1, char *buff2, uint16_t len ){
	if ( len == 0 ) return pdFALSE;
	while(len--) {
		if ( *buff1 != *buff2 ) return pdFALSE;
		buff1++;
		buff2++;
	}
	return pdTRUE;
}
