#include "../Bsp/bsp_wifi.h"

WIFI_STR w_str;

static int connect_wifi( void ) {
	printf("wifi连接中...\r\n");
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.txBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_NAME, WIFI_PASSWD );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	while( pdTRUE ) {
		vTaskDelay(10);
		if ( strstr( (char*)w_str.rxBuff, "OK" ) != NULL ) {
			HAL_UART_AbortReceive( &WIFIHUART );
			printf( "wifi连接成功!\r\n" );
			return 0;
		} else if ( strstr( (char*)w_str.rxBuff, "FAIL" ) != NULL ) {
			HAL_UART_AbortReceive( &WIFIHUART );
			printf( "wifi连接失败!\r\n" );
			return -1;
		}
	}
}

static int send_at_commond( char* cmd, char* reply, uint16_t timeout_10ms ) {
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "%s", cmd );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	for ( ; strstr( (void*)w_str.rxBuff, reply )==NULL && timeout_10ms>0; timeout_10ms-- ) {
		vTaskDelay(10);
	}
	if ( timeout_10ms > 0 ) {
		return 0;
	} else {
		return -1;
	}
}

//连接TCP0
void esp_connect_tcp0 ( void ) {
	uint16_t i;
	if ( w_str.tcp0_errnum++ > 3 ) {//重启设备
		__set_FAULTMASK(1);//重启
		NVIC_SystemReset();
	}
	printf("TCP0 连接中...\r\n");
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "AT+CIPSTART=0,\"TCP\",\"%s\",%d\r\n", TCP_URL_0, TCP_PORT_0 );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	for ( i=1000; i>0; i-- ) {
		if ( strstr( (void*)w_str.rxBuff, "CONNECT" ) != NULL ) {
			printf("TCP0 连接成功\r\n");
			w_str.tcp0_errnum = 0;
			break;
		} else if ( strstr( (void*)w_str.rxBuff, "DNS Fail" ) != NULL ) {
			printf("TCP0 DNS Fail 连接失败\r\n");
			xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP0_DELAY, eSetBits );
			printf("tcp0 断线 5s后重连...\r\n");
			break;
		} else if ( strstr( (void*)w_str.rxBuff, "CLOSED" ) != NULL ) {
			printf("TCP0 CLOSED 连接失败\r\n");
			printf("tcp0 断线 5s后重连...\r\n");
			xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP0_DELAY, eSetBits );
			break;
		}
		vTaskDelay(10);
	}
	if ( i == 0 ) {
		printf("TCP0 连接超时 5s后重连...\r\n");
		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP0_DELAY, eSetBits );
	}
}

//连接TCP1
void esp_connect_tcp1 ( void ) {
	uint16_t i;
	if ( w_str.tcp1_errnum++ > 3 ) {//重启设备
		__set_FAULTMASK(1);//重启
		NVIC_SystemReset();
	}
	printf("TCP1 连接中...\r\n");
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "AT+CIPSTART=1,\"TCP\",\"%s\",%d\r\n", TCP_URL_1, TCP_PORT_1 );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	for ( i=1000; i>0; i-- ) {
		if ( strstr( (void*)w_str.rxBuff, "CONNECT" ) != NULL ) {
			printf("TCP1 连接成功\r\n");
			w_str.tcp1_errnum = 0;
			break;
		} else if ( strstr( (void*)w_str.rxBuff, "DNS Fail" ) != NULL ) {
			printf("TCP1 DNS Fail 连接失败\r\n");
			printf("tcp1 断线 5s后重连...\r\n");
			xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP1_DELAY, eSetBits );
			break;
		} else if ( strstr( (void*)w_str.rxBuff, "CLOSED" ) != NULL ) {
			printf("TCP1 CLOSED 连接失败\r\n");
			printf("tcp1 断线 5s后重连...\r\n");
			xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP1_DELAY, eSetBits );
			break;
		}
		vTaskDelay(10);
	}
	if ( i == 0 ) {
		printf("TCP1 连接超时 5s后重连...\r\n");
		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP1_DELAY, eSetBits );
	}
}

//WIFI初始化
void wifi_init( void ) {

	//复位ESP
	HAL_GPIO_WritePin( WIFI_EN_GPIO_Port, WIFI_EN_Pin, GPIO_PIN_SET );
	HAL_GPIO_WritePin( WIFI_RST_GPIO_Port, WIFI_RST_Pin, GPIO_PIN_RESET );
	vTaskDelay( 100 );
	HAL_GPIO_WritePin( WIFI_RST_GPIO_Port, WIFI_RST_Pin, GPIO_PIN_SET );
	vTaskDelay( 2000 );
	
	memset( &w_str, 0, sizeof( WIFI_STR ) );	
	//DMA初始化
	w_str.txBuff = pvPortMalloc( WIFI_TXBUFF_SIZE );
	w_str.rxBuff = pvPortMalloc( WIFI_RXBUFF_SIZE );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	//开启串口中断
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_TC );
	HAL_UART_DMAStop( &WIFIHUART );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	
	//连接wifi
	connect_wifi();
	//station模式
	send_at_commond( "AT+CWMODE=1\r\n", "OK", 50 );
	//开启多连接
	send_at_commond( "AT+CIPMUX=1\r\n", "OK", 50 );
	//连接TCP
	esp_connect_tcp0();
	esp_connect_tcp1();
	
	HAL_UART_DMAStop( &WIFIHUART );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	
//debug
#if 0
	w_str.isConfig = 1;
	for ( int i=1; i>0; ) {
		w_str.askConfig = 0;
		memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
		HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
		while( w_str.askConfig==0 ) { vTaskDelay(10); } vTaskDelay(20);
		HAL_UART_AbortReceive( &WIFIHUART );
		w_str.dLen = WIFI_RXBUFF_SIZE - __HAL_DMA_GET_COUNTER( &WIFI_HDMA_HUART_RX );
		HAL_UART_Transmit( &huart1, w_str.rxBuff, w_str.dLen, 1000 );
	}
#endif
}

//解析WIFI数据
void wifi_data_parse( void ) {
	uint16_t len;
	if ( strstr( (char*)w_str.rxBuff, "+IPD,0" ) != NULL ) {//TCP0消息
		
	} else if ( strstr( (char*)w_str.rxBuff, "+IPD,1" ) != NULL ) {//TCP1消息
		
	} else if ( strstr( (char*)w_str.rxBuff, "0,CLOSED" ) != NULL ) {//TCP0断开连接
		printf("tcp0 断线 5s后重连...\r\n");
		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP0_DELAY, eSetBits );
		
	} else if ( strstr( (char*)w_str.rxBuff, "1,CLOSED" ) != NULL ) {//TCP1断开连接
		printf("tcp1 断线 5s后重连...\r\n");
		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP1_DELAY, eSetBits );
	}
	
	HAL_UART_AbortReceive( &WIFIHUART );
	len = WIFI_RXBUFF_SIZE - __HAL_DMA_GET_COUNTER(&WIFI_HDMA_HUART_RX);
	memset( w_str.rxBuff, 0, len );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
}














