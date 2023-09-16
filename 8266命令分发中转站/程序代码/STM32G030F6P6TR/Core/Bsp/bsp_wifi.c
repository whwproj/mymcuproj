#include "../Bsp/bsp_wifi.h"

WIFI_STR w_str;

static int connect_wifi( void ) {
	uint8_t timeout = 10;
	sprintf( (char*)w_str.txBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_NAME, WIFI_PASSWD );
	HAL_UART_Receive_DMA( &huart2, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &huart2, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	while( timeout-- ) {	
		w_str.askConfig = 0;
		while( !w_str.askConfig ){;;}
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		w_str.len = WIFI_TXBUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( buffCompareToBuff( "ERROR\r\n", (char *)&w_str.rxBuff[w_str.len - strlen("ERROR\r\n")], strlen("ERROR\r\n") ) ) {
			printf( "wifi连接失败!\r\n" );
			return -1;
		} else if ( buffCompareToBuff( "OK\r\n", (char *)&w_str.rxBuff[w_str.len - strlen("OK\r\n")], strlen("OK\r\n") ) ) {
			printf( "wifi连接成功!\r\n" );
			return 0;
		} else {
			HAL_UART_AbortReceive( &huart2 );
			HAL_UART_Receive_DMA( &huart2, w_str.rxBuff, WIFI_RXBUFF_SIZE );//重启DMA接收
		}
	}
	return -1;
}

static int send_at_commond( char* cmd, char* reply ) {
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "%s", cmd );
	HAL_UART_Receive_DMA( &huart2, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &huart2, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	while( strstr( (void*)w_str.rxBuff, "OK" )==NULL ){;;}
}
	
	

static int connect_tcp( void ) {
	//station模式
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "AT+CWMODE=1" );
	HAL_UART_Receive_DMA( &huart2, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &huart2, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	while( strstr( (void*)w_str.rxBuff, "OK" )==NULL ){;;}
	
	//开启多连接
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "AT+CIPMUX=1" );
	HAL_UART_Receive_DMA( &huart2, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &huart2, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	while( strstr( (void*)w_str.rxBuff, "OK" )==NULL ){;;}
	
	//
		
	//建立TCP连接
		
	
}

void wifi_init( void ) {
	
	//初始化结构体
	memset( &wifi_str, 0, sizeof( WIFI_STR ) );
	
	//初始化串口
	wifi_str.txBuff = pvPortMalloc( WIFI_TXBUFF_SIZE );
	wifi_str.rxBuff = pvPortMalloc( WIFI_RXBUFF_SIZE );
	HAL_UART_DMAStop( &huart1 );
	HAL_UART_Receive_DMA( &huart1, wifi_str.rxBuff, WIFI_RXBUFF_SIZE );
	
	//连接wifi
	connect_wifi();
	
	//连接tcp
	
	
}
















