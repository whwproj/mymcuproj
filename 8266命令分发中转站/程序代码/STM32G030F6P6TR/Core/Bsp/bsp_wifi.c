#include "../Bsp/bsp_wifi.h"

WIFI_STR w_str;
TCP_DATA tcp_data[3];

static int connect_wifi( const char* const wifiName, const char* const wifiPasswd );
static int send_at_commond( char* cmd, char* reply, uint16_t timeout_10ms );
static void wifi_dma_init( uint8_t mode );
static void wifi_dma_reinit( void );
static void wifi_tcp_send_data( uint8_t pid );

//wifi复位
void wifi_reset( void ) {
	memset( &w_str, 0, sizeof( WIFI_STR ) );	
	//复位ESP
	HAL_GPIO_WritePin( ESP_EN_GPIO_Port, ESP_EN_Pin, GPIO_PIN_SET );
	HAL_GPIO_WritePin( ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET );
	vTaskDelay( 100 );
	HAL_GPIO_WritePin( ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_SET );
	vTaskDelay( 1000 );
}

//station模式初始化
void station_mode_init( void ) {
	wifi_dma_init( STATION_MODE );
	if ( connect_wifi( udata.ssid, udata.pswd ) == 0 ) {//连接成功
		printf("wifi连接连接成功\r\n");
		send_at_commond( "AT+CWMODE=1\r\n", "OK", 50 );
		send_at_commond( "AT+CIPMUX=1\r\n", "OK", 50 );
		//wifi连接变更,存入flash
		if ( w_str.updateLink == 1 ) { write_data_into_flash(); }
		
	} else {
		//连接失败,转入station+AP模式
		printf("连接失败,转入station+AP模式\r\n");
		wifi_dma_reinit();
		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STA_AP_MODE_INIT, eSetBits );
	}
}

//station+AP模式初始化,供用户设置wifi账号密码并存入flash
void station_and_ap_init( void ) {
	printf("station+AP模式初始化...\r\n");
	wifi_dma_init( STATION_AP_MODE );
	send_at_commond( "AT+CWMODE=3\r\n", "OK", 50 );
	send_at_commond( "AT+CIPMUX=1\r\n", "OK", 50 );
	send_at_commond( "AT+CIPSERVER=1,8266\r\n", "OK", 50 );
	
	HAL_UART_DMAStop( &WIFIHUART );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_APRXBUFF_SIZE );
	
}


//wifi串口空闲中断回调执行函数
void wifi_uart_idle_callback( void ) {
	uint16_t len;
	char *str1, *str2;
	TCP_DATA *tcpd;
	uint8_t pid;
	if ( w_str.wifiMode == STATION_AP_MODE ) {
		
		if ( strstr( (char*)w_str.rxBuff, "+STA_CONNECTED:" ) ) {
			printf("设备WIFI连接AP\r\n");
			
		} else if ( strstr( (char*)w_str.rxBuff, "GET / HTTP" ) || strstr( (char*)w_str.rxBuff, "GET /? HTTP" ) ) {
			pid = *(strstr((char*)w_str.rxBuff,"+IPD,")+5)-'0';
			printf("TCP%d 建立连接\r\n", pid);
			len = strlen(HTML_BODY_START) + strlen(HTML_CONTENT_1) + strlen(HTML_BODY_END);
			sprintf( (char*)w_str.txBuff, "%s%d%s%s%s%s", (char*)&HTML_HEAD_START, len, (char*)&HTML_HEAD_END,
				(char*)&HTML_BODY_START, (char*)&HTML_CONTENT_1, (char*)&HTML_BODY_END );
			wifi_tcp_send_data( pid );
			
		} else if ( strstr( (char*)w_str.rxBuff, "POST / HTTP" ) ) {
			pid = *(strstr((char*)w_str.rxBuff,"+IPD,")+5)-'0';
			str1 = strstr( (char*)w_str.rxBuff, "wssid=" ) + strlen("wssid=");
			str2 = strstr( (char*)w_str.rxBuff, "&wpswd=" );
			if ( str1!=NULL && str2!=NULL ) {
				memset( udata.ssid, 0, 33 );
				memset( udata.pswd, 0, 21 );
				memcpy( udata.ssid, str1, (uint32_t)str2-(uint32_t)str1 );
				sprintf( udata.pswd, "%s", str2+strlen("&wpswd=") );
				printf("设置wifi参数:");
				printf("udata.ssid: %s\r\n", udata.ssid);
				printf("udata.pswd: %s\r\n", udata.pswd);
				len = strlen(HTML_BODY_START) + strlen(HTML_CONTENT_2) + strlen(HTML_BODY_END);
				sprintf( (char*)w_str.txBuff, "%s%d%s%s%s%s", (char*)&HTML_HEAD_START, len, (char*)&HTML_HEAD_END,
					(char*)&HTML_BODY_START, (char*)&HTML_CONTENT_2, (char*)&HTML_BODY_END );
				wifi_tcp_send_data( pid );
				//重连WIFI
				w_str.updateLink = 1;
				wifi_dma_reinit();
				xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STATION_MODE_INIT, eSetBits );
			}
		}
	
	} else if ( w_str.wifiMode == STATION_MODE ) {
		
	}

//	else if ( strstr( (char*)w_str.rxBuff, "+IPD,0," ) != NULL ) {//TCP0消息(MQTT)
//		
//		printf("IPD 0,----- html\r\n");
//		//str = strstr( (char*)w_str.rxBuff, "+IPD,0," ) + 7;
//		//if ( strstr((char*)w_str.rxBuff, ":GET / HTTP/1.1") != NULL ) {		
//			sprintf( (char*)w_str.txBuff, "%s", (char*)&HTML );
//			w_str.tLen = strlen( (char*)&HTML );//358;
//			//HAL_UART_Transmit( &WIFIHUART, (const uint8_t *)HTML, 358, 500 );
//			wifi_tcp0_send_data();
//		//}
//		
//		for ( ; *str>='0'&&*str<='9'; str++ ) {
//			len *= 10;
//			len += (*str-'0');
//		} str++;
//		//寻找空闲缓存空间
//		for( int i=0; pdTRUE; i++ ) {
//			if ( i==3 ) i=0;
//			if ( tcp_data[i].len == 0 ) {
//				tcpd = &tcp_data[i];
//				break;
//			}
//			vTaskDelay(100);
//		}
//		tcpd->data = pvPortMalloc( len );
//		tcpd->len = len;
//		memcpy( tcpd->data, str, len );
//		xQueueSend( TcpData_QueueHandle, &tcpd, portMAX_DELAY );
		
//	} else if ( strstr( (char*)w_str.rxBuff, "+IPD,1," ) != NULL ) {//TCP1消息
//		//+IPD,1,5:11111
//		str = strstr( (char*)w_str.rxBuff, "+IPD,1," ) + 7;
//		for ( ; *str>='0' && *str<='9'; str++ ) {
//			len *= 10;
//			len += (*str-'0');
//		} str++;
//		printf("TCP1>>>>>>>>>\r\n");
//		HAL_UART_Transmit( &huart1, (uint8_t *)str, len, 1000 );
//		printf("\r\n<<<<<<<<<<<<\r\n");
//		
//	} else if ( strstr( (char*)w_str.rxBuff, "0,CLOSED" ) != NULL ) {//TCP0断开连接
//		printf("tcp0 断线 5s后重连...\r\n");
//		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP0_DELAY, eSetBits );
//		
//	} else if ( strstr( (char*)w_str.rxBuff, "1,CLOSED" ) != NULL ) {//TCP1断开连接
//		printf("tcp1 断线 5s后重连...\r\n");
//		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP1_DELAY, eSetBits );
//	}
//}
	HAL_UART_AbortReceive( &WIFIHUART );
	if ( w_str.wifiMode == STATION_MODE ) {
		len = WIFI_RXBUFF_SIZE - __HAL_DMA_GET_COUNTER( &WIFI_HDMA_HUART_RX );
		memset( w_str.rxBuff, 0, len );
		HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	} else {
		len = WIFI_APRXBUFF_SIZE - __HAL_DMA_GET_COUNTER( &WIFI_HDMA_HUART_RX );
		memset( w_str.rxBuff, 0, len );
		HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_APRXBUFF_SIZE );
	}
}





//建立mqtt连接
void mqtt_connect( void ) {

}


//连接TCP0
void esp_connect_tcp0 ( void ) {
	uint16_t i;	
	if ( w_str.tcp0_errnum++>2 && w_str.tcp1_errnum>2 ) {//重启设备
		//__set_FAULTMASK(1);
		NVIC_SystemReset();//重启
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
			break;
		} else if ( strstr( (void*)w_str.rxBuff, "DNS Fail" ) != NULL ) {
			printf("TCP0 DNS Fail 连接失败\r\n");
			i=0; break;
		} else if ( strstr( (void*)w_str.rxBuff, "CLOSED" ) != NULL ) {
			printf("TCP0 CLOSED 连接失败\r\n");
			i=0; break;
		}
		vTaskDelay(10);
	}
	if ( i == 0 ) {
		printf("TCP0 连接超时 5s后重连...\r\n");
		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP0_DELAY, eSetBits );
	}
	w_str.tcp0_errnum = 0;
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
}

//连接TCP1
void esp_connect_tcp1 ( void ) {
	uint16_t i;
	if ( w_str.tcp1_errnum++>2 && w_str.tcp0_errnum>2 ) {//重启设备
		//__set_FAULTMASK(1);
		NVIC_SystemReset();
	}
	printf("TCP1 连接中...\r\n");
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE ); 
	sprintf( (char*)w_str.txBuff, "AT+CIPSTART=1,\"TCP\",\"%s\",%d\r\n", udata.tcpUrl, udata.tcpPort );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	for ( i=1000; i>0; i-- ) {
		if ( strstr( (void*)w_str.rxBuff, "CONNECT" ) != NULL ) {
			printf("TCP1 连接成功\r\n");
			break;
		} else if ( strstr( (void*)w_str.rxBuff, "DNS Fail" ) != NULL ) {
			printf("TCP1 DNS Fail 连接失败\r\n");
			i=0; break;
		} else if ( strstr( (void*)w_str.rxBuff, "CLOSED" ) != NULL ) {
			printf("TCP1 CLOSED 连接失败\r\n");
			i=0; break;
		}
		vTaskDelay(10);
	}
	if ( i == 0 ) {
		printf("TCP1 连接超时 5s后重连...\r\n");
		xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP1_DELAY, eSetBits );
	}
	w_str.tcp1_errnum = 0;
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
}






#if 0
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
//			jsonStr = cjson_pase_method( tcpd->data, &retain, NULL );
//			if ( jsonStr != NULL ) {
//				//获取信号量
//				xSemaphoreTake( TxSend_Mutex, portMAX_DELAY );
//				//封装mqtt报文
//				w_str.tLen = GetDataPUBLISH( (unsigned char *)w_str.txBuff, 0, 2, 0, pTopicBuff, jsonStr );
//				vPortFree( jsonStr );
//				xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_TCP0_SEND, eSetBits );
//			}
		}
		//释放缓存
		vPortFree( tcpd->data );
		tcpd->len = 0;
	}
	return uxQueueMessagesWaiting( TcpData_QueueHandle );
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
#endif



//多链路发送tcp数据
static void wifi_tcp_send_data( uint8_t pid ) {
	char temp[17];
	uint16_t len_t, timeout;
	
	//关闭中断
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_TC );
	
	len_t = strlen((char*)w_str.txBuff);
	sprintf( temp, "AT+CIPSEND=%d,%d\r\n", pid, len_t );
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, (uint8_t*)temp, strlen(temp), portMAX_DELAY );
	for( timeout=100; strstr( (char*)w_str.rxBuff, "OK\r\n>" )==NULL && timeout>0; timeout-- ) { osDelay(10); }
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, len_t, portMAX_DELAY );
	for( timeout=100; strstr( (char*)w_str.rxBuff, "SEND OK" )==NULL && timeout>0; timeout-- ) { osDelay(10); }
	if ( !timeout ) { printf("SEND ERR\r\n"); } else { printf("SEND OK\r\n"); }
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_TC );
}




//连接wifi
static int connect_wifi( const char* const wifiName, const char* const wifiPasswd ) {
	int res = -1;
	printf("wifi连接中...\r\n");
	//关中断
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_TC );
	HAL_UART_DMAStop( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", wifiName, wifiPasswd );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	while( pdTRUE ) {
		vTaskDelay(10);
		if ( strstr( (char*)w_str.rxBuff, "OK" ) != NULL ) {
			printf( "wifi连接成功!\r\n" );
			res = 0;
			break;
		} else if ( strstr( (char*)w_str.rxBuff, "ERROR" ) != NULL ) {
			printf( "wifi连接失败!\r\n" );
			break;
		} else if ( strstr( (char*)w_str.rxBuff, "FAIL" ) != NULL ) {
			printf( "wifi连接失败!\r\n" );
			break;
		}
	}
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	return res;
}

//设置at命令
static int send_at_commond( char* cmd, char* reply, uint16_t timeout_10ms ) {
	int res = -1;
	//关中断
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_TC );
	HAL_UART_DMAStop( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	sprintf( (char*)w_str.txBuff, "%s", cmd );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	for ( ; strstr( (void*)w_str.rxBuff, reply )==NULL && timeout_10ms>0; timeout_10ms-- ) {
		vTaskDelay(10);
	}
	if ( timeout_10ms > 0 ) {
		res = 0;
	}
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	//开中断
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_TC );
	return res;
}



//DMA初始化
static void wifi_dma_init( uint8_t mode ) {
	if( mode == STATION_MODE ) {
		w_str.wifiMode = STATION_MODE;
		w_str.txBuff = pvPortMalloc( WIFI_TXBUFF_SIZE );
		w_str.rxBuff = pvPortMalloc( WIFI_RXBUFF_SIZE );
		memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	} else if ( mode == STATION_AP_MODE ) {
		w_str.wifiMode = STATION_AP_MODE;
		w_str.txBuff = pvPortMalloc( WIFI_APTXBUFF_SIZE );
		w_str.rxBuff = pvPortMalloc( WIFI_APRXBUFF_SIZE );
		memset( w_str.rxBuff, 0, WIFI_APRXBUFF_SIZE );
	} else {
		return;
	}
	//开启串口中断
	HAL_UART_DMAStop( &WIFIHUART );
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_TC );
}

//DMA反初始化
static void wifi_dma_reinit( void ) {
	//关闭串口中断
	HAL_UART_DMAStop( &WIFIHUART );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_TC );	
	vPortFree( w_str.txBuff );
	vPortFree( w_str.rxBuff );
}

