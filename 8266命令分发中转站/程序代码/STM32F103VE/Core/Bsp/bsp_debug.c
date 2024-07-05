#include "../Bsp/bsp_debug.h"

#define ds debug_str

DEBUG_STR debug_str;

void debug_init( void ) {
	taskENTER_CRITICAL();
	ds.rxBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.txBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	//����DMA���� �������ڿ����ж�
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_IDLE );//ʹ�ܿ����ж�
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_TC );//ʹ�ܷ�������ж�
	HAL_UART_AbortReceive( &huart1 );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );//����DMA����
}


void debug_parse_data_fun( void ) {
	vTaskDelay( 50 );
	HAL_UART_AbortReceive( &huart1 );
	
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	//�ж�����
	if ( strstr( (char *)ds.rxBuff, "abcd" ) != NULL ) {
		printf("abcd2\r\n");
	} else if ( strstr( (char *)ds.rxBuff, "esp:" ) != NULL ) {
		HAL_UART_Transmit( &huart3, &ds.rxBuff[4], ds.len-4, 1000 );
	} else {
		
	}
	memset( ds.rxBuff, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );//����DMA����
}

