#include "../Bsp/bsp_wifi.h"

WIFI_STR w_str;
TCP_DATA tcp_data[3];

static int connect_wifi( const char* const wifiName, const char* const wifiPasswd ) {
	printf("wifi连接中...\r\n");
	w_str.isConfig = 1;
	//xSemaphoreTake( communication_Mutex, portMAX_DELAY );//获取信号量
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", wifiName, wifiPasswd );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	while( pdTRUE ) {
		vTaskDelay(10);
		if ( strstr( (char*)w_str.rxBuff, "OK" ) != NULL ) {
			HAL_UART_AbortReceive( &WIFIHUART );
			printf( "wifi连接成功!\r\n" );
			HAL_UART_AbortReceive( &WIFIHUART );
			memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
			//xSemaphoreGive( communication_Mutex );//释放信号量
			w_str.isConfig = 0;
			return 0;
		} else if ( strstr( (char*)w_str.rxBuff, "FAIL" ) != NULL ) {
			HAL_UART_AbortReceive( &WIFIHUART );
			printf( "wifi连接失败!\r\n" );
			HAL_UART_AbortReceive( &WIFIHUART );
			memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
			//xSemaphoreGive( communication_Mutex );//释放信号量
			w_str.isConfig = 0;
			return -1;
		}
	}
}


static int send_at_commond( char* cmd, char* reply, uint16_t timeout_10ms ) {
	//xSemaphoreTake( communication_Mutex, portMAX_DELAY );//获取信号量
	w_str.isConfig = 1;
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "%s", cmd );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	for ( ; strstr( (void*)w_str.rxBuff, reply )==NULL && timeout_10ms>0; timeout_10ms-- ) {
		vTaskDelay(10);
	}
	if ( timeout_10ms > 0 ) {
		HAL_UART_AbortReceive( &WIFIHUART );
		memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
		//xSemaphoreGive( communication_Mutex );//释放信号量
		w_str.isConfig = 0;
		return 0;
	} else {
		HAL_UART_AbortReceive( &WIFIHUART );
		memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
		//xSemaphoreGive( communication_Mutex );//释放信号量
		w_str.isConfig = 0;
		return -1;
	}
}

//连接TCP0
void esp_connect_tcp0 ( void ) {
	uint16_t i;	
	if ( w_str.tcp0_errnum++>2 && w_str.tcp1_errnum>2 ) {//重启设备
		__set_FAULTMASK(1);//重启
		NVIC_SystemReset();
	}
	printf("TCP0 连接中...\r\n");
	//xSemaphoreTake( communication_Mutex, portMAX_DELAY );//获取信号量
	w_str.isConfig = 1;
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
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	//xSemaphoreGive( communication_Mutex );//释放信号量
	w_str.isConfig = 0;
}

//连接TCP1
void esp_connect_tcp1 ( void ) {
	uint16_t i;
	if ( w_str.tcp1_errnum++>2 && w_str.tcp0_errnum>2 ) {//重启设备
		__set_FAULTMASK(1);//重启
		NVIC_SystemReset();
	}
	//xSemaphoreTake( communication_Mutex, portMAX_DELAY );//获取信号量
	w_str.isConfig = 1;
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
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	//xSemaphoreGive( communication_Mutex );//释放信号量
	w_str.isConfig = 0;
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
	if ( connect_wifi( WIFI_NAME0, WIFI_PASSWD0 ) != 0 ) {
		if ( connect_wifi( WIFI_NAME1, WIFI_PASSWD1 ) != 0 ) {
			printf("wifi err\r\n");
		}
	}
	//station模式
	send_at_commond( "AT+CWMODE=1\r\n", "OK", 50 );
	//开启多连接
	send_at_commond( "AT+CIPMUX=1\r\n", "OK", 50 );
	//连接TCP
	esp_connect_tcp1();
	esp_connect_tcp0();
	//mqtt建立连接
	mqtt_connect();
	
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
void wifi_data_classification( void ) {
	uint16_t len;
	char *str;
	TCP_DATA *tcpd;
	//xSemaphoreTake( communication_Mutex, portMAX_DELAY );//获取信号量
	if ( strstr( (char*)w_str.rxBuff, "+IPD,0," ) != NULL ) {//TCP0消息(MQTT)
		str = strstr( (char*)w_str.rxBuff, "+IPD,0," ) + 7;
		for ( ; *str>='0'&&*str<='9'; str++ ) {
			len *= 10;
			len += (*str-'0');
		} str++;
		//获取空闲缓存信号量
		xSemaphoreGive( TcpRxCache_Mutex );
		//寻找空闲缓存空间
		for( int i=0; pdTRUE; i++ ) {
			if ( i==3 ) i=0;
			if ( tcp_data[i].len == 0 ) {
				tcpd = &tcp_data[i];
				break;
			}
			vTaskDelay(100);
		}
		memcpy( tcpd->data, str, len );
		xQueueSend( TcpData_QueueHandle, &tcpd, portMAX_DELAY );
		
	} else if ( strstr( (char*)w_str.rxBuff, "+IPD,1," ) != NULL ) {//TCP1消息
		//+IPD,1,5:11111
		str = strstr( (char*)w_str.rxBuff, "+IPD,1," ) + 7;
		for ( ; *str>='0' && *str<='9'; str++ ) {
			len *= 10;
			len += (*str-'0');
		} str++;
		printf("TCP1>>>>>>>>>\r\n");
		HAL_UART_Transmit( &huart1, (uint8_t *)str, len, 1000 );
		printf("\r\n<<<<<<<<<<<<\r\n");
		
	} else if ( strstr( (char*)w_str.rxBuff, "0,CLOSED" ) != NULL ) {//TCP0断开连接
		printf("tcp0 断线 5s后重连...\r\n");
		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP0_DELAY, eSetBits );
		
	} else if ( strstr( (char*)w_str.rxBuff, "1,CLOSED" ) != NULL ) {//TCP1断开连接
		printf("tcp1 断线 5s后重连...\r\n");
		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP1_DELAY, eSetBits );
	}
	
	HAL_UART_AbortReceive( &WIFIHUART );
	len = WIFI_RXBUFF_SIZE - __HAL_DMA_GET_COUNTER( &WIFI_HDMA_HUART_RX );
	memset( w_str.rxBuff, 0, len );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	
	//xSemaphoreGive( communication_Mutex );//释放信号量
}

//解析mqtt数据
UBaseType_t wifi_mqtt_data_parse( void ) {
	uint8_t i, retain, ask[4];
	char *jsonStr;
	TCP_DATA *tcpd;
	
	//读取队列消息
	if ( xQueueReceive( TcpData_QueueHandle, &tcpd, pdFALSE ) ) {
		//判断报文类型,普通应答跳过mqtt解析,直接封包
		if ( tcpd->data[0]==0x62 ) {//作为接收端应答报文(直接发送到tls加密队列)
			//获取信号量
			xSemaphoreTake( TxSend_Mutex, portMAX_DELAY );
			//封装心跳包
			w_str.rxBuff[0]=0x70; w_str.rxBuff[1]=0x02;
			w_str.rxBuff[2]=tcpd->data[2]; w_str.rxBuff[3]=tcpd->data[3];
			w_str.tLen = 4;
			//发送信号
			xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_TCP0_SEND, eSetBits );

		} else if ( (tcpd->data[0]&0xF0)==0x50 ) {//作为发送端应答报文(直接发送到tls加密队列)
			//获取信号量
			xSemaphoreTake( TxSend_Mutex, portMAX_DELAY );
			//封装应答包
			ask[0]=0x62; ask[1]=0x02;
			ask[2]=tcpd->data[2]; ask[3]=tcpd->data[3];
			w_str.tLen = 4;
			//发送信号
			xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_TCP0_SEND, eSetBits );

		} else if ( tcpd->data[0]==0xD0 && tcpd->data[1]==0x00 ) {//收到心跳包应答
			printf("------ MQTT__应答心跳包 \r\n");
			w_str.heartBeatTime = 0;
			
		} else if ( (tcpd->data[0]&0xF0)==0x30 ) {//mqtt数据
			retain = 0;//保留消息默认0不保留
			//解析处理json
			jsonStr = cjson_pase_method( tcpd->data, &retain, NULL );
			if ( jsonStr != NULL ) {
				//获取信号量
				xSemaphoreTake( TxSend_Mutex, portMAX_DELAY );
				//封装mqtt报文
				w_str.tLen = GetDataPUBLISH( (unsigned char *)w_str.txBuff, 0, 2, 0, pTopicBuff, jsonStr );
				vPortFree( jsonStr );
				xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_TCP0_SEND, eSetBits );
			}
		}
	}
	return uxQueueMessagesWaiting( TcpData_QueueHandle );
}


void wifi_tcp0_send_data( void ) {
	char temp[17];
	//关闭中断
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_TC );
	
	sprintf( temp, "AT+CIPSEND=0,%d", w_str.tLen );
	HAL_UART_Transmit( &WIFIHUART, (uint8_t*)temp, strlen(temp), portMAX_DELAY );
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	while( strstr( (char*)w_str.rxBuff, "OK\r\n>" ) == NULL );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, w_str.tLen, portMAX_DELAY );
	while( strstr( (char*)w_str.rxBuff, "SEND OK" ) == NULL );
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	//释放信号量
	xSemaphoreGive( TxSend_Mutex );
	
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_TC );
}

void wifi_mqtt_heart( void ) {
	//获取信号量
	xSemaphoreTake( TxSend_Mutex, portMAX_DELAY );
	//封装心跳包
	GetDataPINGREQ( w_str.rxBuff );
	//发送信号
	xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_TCP0_SEND, eSetBits );
}

void mqtt_connect( void ) {
	//获取信号量
	xSemaphoreTake( TxSend_Mutex, portMAX_DELAY );
	
	//连接mqtt服务器
	GetDataConnet( w_str.rxBuff );
	
	//订阅主题
	GetDataSUBSCRIBE( m.mqttTxBuff, net_str.servicesTopic, 2 );
	mqttConCache = mbedtls_parse_data_mqtt_con( m.mqttTxBuff, m.mqttTxBuff[1]+2 );
	if ( mqttConCache==NULL || mqttConCache[0]!=0x90 ) {
		return pdFAIL;//订阅失败
	}
}







