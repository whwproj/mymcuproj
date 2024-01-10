#include "../Bsp/bsp_debug.h"

#define ds debug_str

DEBUG_STR debug_str;

void debug_init( void ) {
	ds.rxBuff = malloc( DEBUG_BUFF_SIZE );
	ds.txBuff = malloc( DEBUG_BUFF_SIZE );
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_IDLE );
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_TC );
	HAL_UART_AbortReceive( &huart1 );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );
}


void debug_parse_data_fun( void ) {
	HAL_Delay( 50 );
	HAL_UART_AbortReceive( &huart1 );
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	if ( strstr( (char *)ds.rxBuff, "init" ) != NULL ) {
		printf("init ok\r\n");
		
	} else if ( strstr( (char *)ds.rxBuff, "xxxx" ) != NULL ) {
		
	} else {
	
	}
	memset( ds.rxBuff, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );
}


