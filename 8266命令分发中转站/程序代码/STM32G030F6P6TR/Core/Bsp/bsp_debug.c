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
		
	} else if ( strstr( (char *)ds.rxBuff, "esp:" ) != NULL ) {
		HAL_UART_Transmit( &huart1, &ds.rxBuff[4], ds.len-4, 1000 );
	}

//	else if ( strstr( (char *)ds.rxBuff, "clear reg" ) != NULL ) {
//		SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xf0 );//0xFF空指令
//		printf("clear reg ok\r\n");

//	} else if ( strstr( (char *)ds.rxBuff, "clear fifo" ) != NULL ) {
//		SPI_RW_Reg( FLUSH_RX,NOP );
//		printf("clear fifo ok\r\n");
//
//	} else if ( strstr( (char *)ds.rxBuff, "reg" ) != NULL ) {
//		uint8_t sta = SPI_RW_Reg(NRF_READ_REG + STATUS, 0xff );//0xFF空指令
//		printf("sta: %02X\r\n", sta);
//
//	} else if ( strstr( (char *)ds.rxBuff, "gpio" ) != NULL ) {
//		uint8_t temp = HAL_GPIO_ReadPin( NRF_IRQ_GPIO_Port, NRF_IRQ_Pin );
//		printf("temp: %d\r\n", temp);
//
//	}
	memset( ds.rxBuff, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff, DEBUG_BUFF_SIZE );//启动DMA接收
}
