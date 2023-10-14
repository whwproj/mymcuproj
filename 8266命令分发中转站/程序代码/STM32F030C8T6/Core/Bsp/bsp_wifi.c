#include "../Bsp/bsp_wifi.h"

WIFI_STR w_str;

static int connect_wifi( const char* const wifiName, const char* const wifiPasswd );
static int send_at_commond( char* cmd, char* reply, uint16_t timeout_10ms );
static void wifi_dma_init( uint8_t mode );
static void wifi_dma_reinit( void );
static void wifi_tcp_send_data( uint8_t pid );
static int post_param_handle( char* param );
//static int esp_connect_tcp0 ( void );
static int esp_connect_tcp1 ( void );
static int mqtt_connect( void );

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
	
	send_at_commond( "AT+RST\r\n", "OK", 50 );
	vTaskDelay(500);
	
	if ( connect_wifi( udata.wssid, udata.wpswd ) == 0 ) {//连接成功
		//printf("wifi连接连接成功\r\n");
		led_nrf_flicker_off(0);
		if ( w_str.updateLink == 1 ) { write_data_into_flash(); }//wifi连接变更,存入flash
		send_at_commond( "AT+CWMODE=1\r\n", "OK", 50 );
		send_at_commond( "AT+CIPMUX=1\r\n", "OK", 50 );
		if ( esp_connect_tcp1() == -1 ) {
			//连接失败,转入station+AP模式
			//printf("连接失败,转入station+AP模式\r\n");
			led_con_flicker_off(1);
			wifi_dma_reinit();
			xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STA_AP_MODE_INIT, eSetBits );
			return;
		}
		//建立mqtt连接,订阅主题
		if ( mqtt_connect() == -1 ) {
			//连接失败,转入station+AP模式
			//printf("mqtt连接失败, 转入station+AP模式\r\n");
			led_con_flicker_off(1);
			wifi_dma_reinit();
			xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STA_AP_MODE_INIT, eSetBits );
			return;
		}
		//mqtt连接成功,主题订阅成功
		led_con_flicker_off(0);
		w_str.mqttSta = 1;//mqtt连接成功,订阅主题成功
		w_str.sendLock = 1;//解锁
		//开中断
		__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_IDLE );
		__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_TC );
		//初始化NRF
		xTaskNotify( nrf_control_taskHandle, 1U<<NRF_INIT_EVENT, eSetBits );
		
	} else {
		//连接失败,转入station+AP模式
		//printf("连接失败,转入station+AP模式\r\n");
		led_nrf_flicker_off(1);
		led_con_flicker_off(1);
		wifi_dma_reinit();
		xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STA_AP_MODE_INIT, eSetBits );
	}
}

//station+AP模式初始化,供用户设置wifi账号密码并存入flash
void station_and_ap_init( void ) {
	send_at_commond( "AT+RST\r\n", "OK", 50 );
	vTaskDelay(500);
	//printf("station+AP模式初始化...\r\n");
	wifi_dma_init( STATION_AP_MODE );
	send_at_commond( "AT+CWMODE=3\r\n", "OK", 50 );
	send_at_commond( "AT+CIPMUX=1\r\n", "OK", 50 );
	send_at_commond( "AT+CIPSERVER=1,8266\r\n", "OK", 50 );
	HAL_UART_DMAStop( &WIFIHUART );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_APRXBUFF_SIZE );
}

//wifi串口空闲中断回调执行函数
void wifi_uart_idle_callback( void ) {
	uint16_t len_t, len_t2, mqttId;
	char *str, *dataStr, *jsonStr, *topic;
	//TCP_DATA *tcpd;
	uint8_t pid, num, qos;
	if ( w_str.wifiMode == STATION_AP_MODE ) {
		
		if ( strstr( (char*)w_str.rxBuff, "+STA_CONNECTED:" ) ) {
			//printf("设备WIFI连接AP\r\n");
			
		} else if ( strstr( (char*)w_str.rxBuff, "GET /" ) ) {
			pid = *(strstr((char*)w_str.rxBuff,"+IPD,")+5)-'0';
			//printf("TCP%d 建立连接\r\n", pid);
			len_t = strlen(HTML_BODY_START) + strlen(HTML_CONTENT_1) + strlen(HTML_BODY_END);
			sprintf( (char*)w_str.txBuff, "%s%d%s%s%s%s", (char*)&HTML_HEAD_START, len_t, (char*)&HTML_HEAD_END,
				(char*)&HTML_BODY_START, (char*)&HTML_CONTENT_1, (char*)&HTML_BODY_END );
			wifi_tcp_send_data( pid|ENABLE_IT );
			
		} else if ( strstr( (char*)w_str.rxBuff, "POST /" ) ) {
			pid = *(strstr((char*)w_str.rxBuff,"+IPD,")+5)-'0';
			str = strstr( (char*)w_str.rxBuff, "\r\n\r\n" ) + 4;
			if ( post_param_handle(str) == 0 ) {
				led_nrf_flicker_on();
				led_con_flicker_on();
				//printf("参数设置成功\r\n");
				len_t = strlen(HTML_BODY_START) + strlen(HTML_CONTENT_2) + strlen(HTML_BODY_END);
				sprintf( (char*)w_str.txBuff, "%s%d%s%s%s%s", (char*)&HTML_HEAD_START, len_t, (char*)&HTML_HEAD_END,
					(char*)&HTML_BODY_START, (char*)&HTML_CONTENT_2, (char*)&HTML_BODY_END );
				wifi_tcp_send_data( pid|ENABLE_IT );
				//重连WIFI
				send_at_commond( "AT+RST\r\n", "OK", 50 );
				vTaskDelay(500);
				w_str.updateLink = 1;
				wifi_dma_reinit();
				xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STATION_MODE_INIT, eSetBits );
			}
		}
	
	} else if ( w_str.wifiMode == STATION_MODE ) {
		while( w_str.sendLock == 0 ){osDelay(5);}
		w_str.sendLock = 0;//上锁不可发
		if ( strstr( (char*)w_str.rxBuff, "+IPD,1," ) != NULL ) {//TCP1消息(MQTT)
			//解析mqtt
			str = strstr((char*)w_str.rxBuff,"+IPD,1,") + 7;
			for ( ; *str>='0'&&*str<='9'; str++ ) {
				len_t *= 10;
				len_t += (*str-'0');
			}str++;
			
			if ( ((uint8_t)str[0]&0xF0) == 0x30 ) {//mqtt数据
				len_t2 = (str[1]&0x7F) << 0;
				num = 2;
				if ( str[1] & 0x80 ) {//进位
					len_t2 |= (str[2]&0x7F) << 7;
					num += 1;
					if ( str[2] & 0x80 ) {//进位
						len_t2 |= (str[3]&0x7F) << 14;
						num += 1;
						if ( str[3] & 0x80 ){//进位
							len_t2 |= str[4] << 21;
							num += 1;
						}
					}
				}
				if ( len_t < (len_t2 + num) ) {
					vTaskDelay(10);
					goto end;
				}
				
				//解析主题,载荷,返回载荷长度
				dataStr = pvPortMalloc(100);//主题长度-主题-标识
				topic = pvPortMalloc(8);
				memset(dataStr, 0, 100);
				memset(topic, 0, 8);
				len_t = mqttParseData( (uint8_t*)str, (uint8_t*)topic, (uint8_t*)dataStr, &qos, &mqttId);
				
				if ( qos == 1 ) {
					w_str.txLen = 4;
					w_str.txBuff[0] = 0x40;
					w_str.txBuff[1] = 0x02;
					w_str.txBuff[2] = mqttId>>8; 
					w_str.txBuff[3] = mqttId;
					wifi_tcp_send_data( 1|NOT_STR );
				} else if ( qos == 2 ) {
					w_str.txLen = 4;
					w_str.txBuff[0] = 0x50;
					w_str.txBuff[1] = 0x02;
					w_str.txBuff[2] = mqttId>>8; 
					w_str.txBuff[3] = mqttId;
					wifi_tcp_send_data( 1|NOT_STR );
				}
				if ( jsonStr != NULL ) {
					xQueueSend( wifi_data_handle, dataStr, portMAX_DELAY );
					xTaskNotify( data_task_handle, 1U<<PARSE_WIFI_DATA, eSetBits );
					vPortFree( dataStr );
					__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_IDLE );
					__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_TC );
					//发送到消息队列
					/*//封装mqtt报文
					w_str.txLen = GetDataPUBLISH( (unsigned char *)(w_str.txBuff), 0, 1, 0, MQTT_PUBTopic, jsonStr );
					vPortFree( jsonStr );
					wifi_tcp_send_data( 1|ENABLE_IT|NOT_STR );
					goto end;*/
				}
				vPortFree( topic );
				
			} else if ( (uint8_t)str[0]==0x62 ) {//作为接收端应答报文
				w_str.txBuff[0]=0x70; w_str.txBuff[1]=0x02;
				w_str.txBuff[2]=str[2]; w_str.txBuff[3]=str[3];
				w_str.txLen = 4;
				wifi_tcp_send_data( 1|ENABLE_IT|NOT_STR );
				goto end;
				
			} else if ( (uint8_t)str[0]==0x50 ) {//作为发送端应答报文
				w_str.txBuff[0]=0x62; w_str.txBuff[1]=0x02;
				w_str.txBuff[2]=str[2]; w_str.txBuff[3]=str[3];
				w_str.txLen = 4;
				wifi_tcp_send_data( 1|ENABLE_IT|NOT_STR );
				goto end;
				
			} else if ( (uint8_t)str[0]==0xD0 && (uint8_t)str[1]==0x00 ) {//收到心跳包应答
				//printf("----应答心跳\r\n");
				w_str.heartBeatTime = 0;
			}
			
		} else {
		
		}
		
	}

	HAL_UART_AbortReceive( &WIFIHUART );
	if ( w_str.wifiMode == STATION_MODE ) {
		len_t = WIFI_RXBUFF_SIZE - __HAL_DMA_GET_COUNTER( &WIFI_HDMA_HUART_RX );
		memset( w_str.rxBuff, 0, len_t );
		HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	} else {
		len_t = WIFI_APRXBUFF_SIZE - __HAL_DMA_GET_COUNTER( &WIFI_HDMA_HUART_RX );
		memset( w_str.rxBuff, 0, len_t );
		HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_APRXBUFF_SIZE );
	}
	
	end:
	w_str.sendLock = 1;//解锁
	return;
}

//发送心跳ISR
void send_mqtt_heart_isr( void ) {
	BaseType_t phpt;
	static uint8_t time = 0;//0.5s 中断一次
	if ( w_str.mqttSta ) {//mqtt在线
		if ( w_str.sendLock && time++>=10 ) {
			time = 0;
			xTaskNotifyFromISR( wifi_control_taskHandle, 1U<<WIFI_SEND_HEART, eSetBits, &phpt );
			portYIELD_FROM_ISR( phpt );
		}
	}
}

//发送心跳
void send_mqtt_heart( void ) {
	if ( w_str.sendLock ) {
		w_str.sendLock = 0;//上锁
		w_str.heartBeatTime++;
		if ( w_str.heartBeatTime > 2 ) {//重连mqtt
			led_nrf_flicker_on();//重连灯闪烁
			led_con_flicker_on();
			wifi_dma_reinit();//重置DMA结构体
			wifi_reset();//WIFI复位
			//重连wifi,mqtt
			xTaskNotify( wifi_control_taskHandle, 1U<<WIFI_STATION_MODE_INIT, eSetBits );
			return;
		}
		GetDataPINGREQ( w_str.txBuff );
		w_str.txLen = 2;
		wifi_tcp_send_data( 1|ENABLE_IT|NOT_STR );
		w_str.sendLock = 1;//解锁
		
	}
}

//连接TCP0
/*static int esp_connect_tcp0 ( void ) {
	uint16_t i;	
	int res = -1;
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
			res = 0;
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
	return res;
}*/

//连接TCP1
static int esp_connect_tcp1 ( void ) {
	uint16_t i;
	int res = -1;
	if ( w_str.tcp1_errnum++>2 && w_str.tcp0_errnum>2 ) {//重启设备
		//__set_FAULTMASK(1);
		NVIC_SystemReset();
	}
	//printf("TCP1 连接中...\r\n");
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE ); 
	sprintf( (char*)w_str.txBuff, "AT+CIPSTART=1,\"TCP\",\"%s\",%d\r\n", udata.tcpurl, udata.tcpport );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, strlen((char*)w_str.txBuff), portMAX_DELAY );
	for ( i=1000; i>0; i-- ) {
		if ( strstr( (void*)w_str.rxBuff, "CONNECT" ) != NULL ) {
			//printf("TCP1 连接成功\r\n");
			res = 1;
			break;
		} else if ( strstr( (void*)w_str.rxBuff, "DNS Fail" ) != NULL ) {
			//printf("TCP1 DNS Fail 连接失败\r\n");
			i=0; break;
		} else if ( strstr( (void*)w_str.rxBuff, "CLOSED" ) != NULL ) {
			//printf("TCP1 CLOSED 连接失败\r\n");
			i=0; break;
		}
		vTaskDelay(10);
	}
	if ( i == 0 ) {
		//printf("TCP1 连接超时 5s后重连...\r\n");
		//xTaskNotify( wifi_tcp_connect_taskHandle, 1U<<WIFI_CONNECT_TCP1_DELAY, eSetBits );
	}
	w_str.tcp1_errnum = 0;
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	return res;
}




static int static_mqtt_commont_t( uint8_t reply ) {
	uint8_t *buff_t = NULL;
	int i;
	w_str.txLen = w_str.txBuff[1]+2;
	wifi_tcp_send_data(1|NOT_STR);
	for ( i=100; buff_t[0]!=reply&&i>0; i-- ) {
		buff_t = (uint8_t*)strstr( (char*)w_str.rxBuff, ":" ) + 1;
		vTaskDelay(5);
	}
	if ( i > 0 ) {
		return 0;
	}
	return -1;
}

//建立mqtt连接
static int mqtt_connect( void ) {
	int res = -1;
	//建立连接
	GetDataConnet( w_str.txBuff );
	if ( static_mqtt_commont_t(0x20) == -1 ) {//连接确认
		//printf("mqtt连接失败\r\n");
		goto end;
	}
	memset( w_str.txBuff, 0, WIFI_TXBUFF_SIZE );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	//printf("mqtt连接成功\r\n");
	
	
	//订阅主题
	GetDataSUBSCRIBE( w_str.txBuff, MQTT_SUB1Topic, 1 );
	if ( static_mqtt_commont_t(0x90) == -1 ) {//订阅确认
		//printf("mqtt订阅主题1失败\r\n");
		goto end;
	}
	memset( w_str.txBuff, 0, WIFI_TXBUFF_SIZE );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	//printf("mqtt订阅主题1成功\r\n");
	
	//订阅主题
	GetDataSUBSCRIBE( w_str.txBuff, MQTT_SUB2Topic, 1 );
	if ( static_mqtt_commont_t(0x90) == -1 ) {//订阅确认
		//printf("mqtt订阅主题2失败\r\n");
		goto end;
	}
	memset( w_str.txBuff, 0, WIFI_TXBUFF_SIZE );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	//printf("mqtt订阅主题2成功\r\n");
	res = 0;
	
	end:
	return res;
}


//多链路发送tcp数据
static void wifi_tcp_send_data( uint8_t pid ) {
	char temp[17];
	uint16_t len_t, timeout;
	
	//关闭中断
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_TC );
	
	if ( !(pid&NOT_STR) ) {//是字符串
		len_t = strlen((char*)w_str.txBuff);
	} else {
		len_t = w_str.txLen;
	}
	sprintf( temp, "AT+CIPSEND=%d,%d\r\n", pid&0x0F, len_t );
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	HAL_UART_Transmit( &WIFIHUART, (uint8_t*)temp, strlen(temp), portMAX_DELAY );
	for( timeout=100; strstr( (char*)w_str.rxBuff, "OK\r\n>" )==NULL && timeout>0; timeout-- ) { osDelay(10); }
	
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, 30 );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );

	HAL_UART_Transmit( &WIFIHUART, w_str.txBuff, len_t, portMAX_DELAY );
	for( timeout=0x5FFF; strstr( (char*)w_str.rxBuff, "SEND OK" )==NULL && timeout>0; timeout-- ) { ;; }
	//if ( !timeout ) { printf("SEND ERR\r\n"); } else { printf("SEND OK\r\n"); }
	
	HAL_UART_AbortReceive( &WIFIHUART );
	memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	HAL_UART_Receive_DMA( &WIFIHUART, w_str.rxBuff, WIFI_RXBUFF_SIZE );
	
	
	if ( pid & ENABLE_IT ) {
		__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_IDLE );
		__HAL_UART_ENABLE_IT( &WIFIHUART, UART_IT_TC );
	}
	
//	printf("timeout: %d\r\n", timeout );
//	for ( int j=0; j<100; j++ ) {
//		printf( "0x%.2X ", w_str.rxBuff[j] );
//	}
//	printf("\r\n");
}




//连接wifi
static int connect_wifi( const char* const wifiName, const char* const wifiPasswd ) {
	int res = -1;
	if ( strlen(wifiName)==0 || (uint8_t)wifiName[0]==0xff ) return res;
	
	//printf("wifi连接中...\r\n");
	
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
			//printf( "wifi连接成功!\r\n" );
			res = 0;
			break;
		} else if ( strstr( (char*)w_str.rxBuff, "ERROR" ) != NULL ) {
			//printf( "wifi连接失败!\r\n" );
			break;
		} else if ( strstr( (char*)w_str.rxBuff, "FAIL" ) != NULL ) {
			//printf( "wifi连接失败!\r\n" );
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
		w_str.rxBuff = pvPortMalloc( WIFI_RXBUFF_SIZE );
		w_str.txBuff = pvPortMalloc( WIFI_TXBUFF_SIZE );
		memset( w_str.rxBuff, 0, WIFI_RXBUFF_SIZE );
	} else if ( mode == STATION_AP_MODE ) {
		w_str.wifiMode = STATION_AP_MODE;
		w_str.rxBuff = pvPortMalloc( WIFI_APRXBUFF_SIZE );
		w_str.txBuff = pvPortMalloc( WIFI_APTXBUFF_SIZE );
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
	HAL_UART_DMAStop( &WIFIHUART );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_IDLE );
	__HAL_UART_DISABLE_IT( &WIFIHUART, UART_IT_TC );	
	vPortFree( w_str.rxBuff );
	vPortFree( w_str.txBuff );
}

//参数处理
static int post_param_handle( char* param ) {
	uint16_t len_t;
	char *str_t, *str, *str1_t, *str2_t;
	int res = -1;//成功更新过,并且校验所有参数合法时res=0
	
	if ( param == NULL ) return res;
	
	//ssid
	str1_t = strstr( param, "wssid=" )+6;
	str2_t = strstr( param, "&wpswd=" );
	len_t = (str1_t!=NULL && str2_t!=NULL) ? (uint16_t)((uint32_t)str2_t - (uint32_t)str1_t) : 0;
	if ( len_t != 0 ) {
		memset( udata.wssid, 0, 33 );
		str_t = pvPortMalloc( len_t+1 );
		memcpy( str_t, str1_t, len_t );
		str_t[len_t] = '\0';
		//URL转义解码,去除%XX
		str = decodeURL( str_t );
		sprintf( udata.wssid, "%s", str );
		vPortFree( str_t );
		vPortFree( str );
		res = 0;
	}
	
	//wssid
	str1_t = str2_t + 7;
	str2_t = strstr( param, "&tcpurl=" );
	len_t = (str1_t!=NULL && str2_t!=NULL) ? (uint16_t)((uint32_t)str2_t - (uint32_t)str1_t) : 0;
	if ( len_t != 0 ) {
		memset( udata.wpswd, 0, 21 );
		str_t = pvPortMalloc( len_t+1 );
		memcpy( str_t, str1_t, len_t );
		str_t[len_t] = '\0';
		//URL转义解码,去除%XX
		str = decodeURL( str_t );
		sprintf( udata.wpswd, "%s", str );
		vPortFree( str_t );
		vPortFree( str );
		res = 0;
	}
	
	//tcpurl
	str1_t = str2_t + 8;
	str2_t = strstr( param, "&tcpport=" );
	len_t = (str1_t!=NULL && str2_t!=NULL) ? (uint16_t)((uint32_t)str2_t - (uint32_t)str1_t) : 0;
	if ( len_t != 0 ) {
		memset( udata.tcpurl, 0, 43 );
		str_t = pvPortMalloc( len_t+1 );
		memcpy( str_t, str1_t, len_t );
		str_t[len_t] = '\0';
		//URL转义解码,去除%XX
		str = decodeURL( str_t );
		sprintf( udata.tcpurl, "%s", str );
		vPortFree( str_t );
		vPortFree( str );
		res = 0;
	}
	
	//tcpport
	str1_t = str2_t + 9;
	str2_t = strstr( param, "&mqusername=" );
	len_t = (str1_t!=NULL && str2_t!=NULL) ? (uint16_t)((uint32_t)str2_t - (uint32_t)str1_t) : 0;
	if ( len_t != 0 ) {
		for ( len_t=0; (*str1_t>='0')&&(*str1_t<='9');  ) {
			len_t *= 10;
			len_t += (*str1_t-'0');
			str1_t++;
		}
		udata.tcpport = len_t;
		res = 0;
	}

	//mqusername
	str1_t = str2_t + 12;
	str2_t = strstr( param, "&mqpasswd=" );
	len_t = (str1_t!=NULL && str2_t!=NULL) ? (uint16_t)((uint32_t)str2_t - (uint32_t)str1_t) : 0;
	if ( len_t != 0 ) {
		memset( udata.mqusername, 0, 51 );
		str_t = pvPortMalloc( len_t+1 );
		memcpy( str_t, str1_t, len_t );
		str_t[len_t] = '\0';
		//URL转义解码,去除%XX
		str = decodeURL( str_t );
		sprintf( udata.mqusername, "%s", str );
		vPortFree( str_t );
		vPortFree( str );
		res = 0;
	}
	
	//mqpasswd
	str1_t = str2_t + 10;
	if ( strlen(str1_t)>0 && (uint8_t)str1_t[0]!=0xFF ) {
		memset( udata.mqpasswd, 0, 33 );
		str_t = pvPortMalloc( 33 );
		sprintf( str_t, "%s", str1_t );
		//URL转义解码,去除%XX
		str = decodeURL( str_t );
		sprintf( udata.mqpasswd, "%s", str );
		vPortFree( str_t );
		vPortFree( str );
		res = 0;
	}
	
	if ( strlen(udata.wssid)==0 || (uint8_t)udata.wssid[0]==0xFF ) res = -1;
	if ( strlen(udata.wpswd)==0 || (uint8_t)udata.wpswd[0]==0xFF ) res = -1;
	if ( strlen(udata.tcpurl)==0 || (uint8_t)udata.tcpurl[0]==0xFF ) res = -1;
	if ( strlen(udata.mqusername)==0 || (uint8_t)udata.mqusername[0]==0xFF ) res = -1;
	if ( strlen(udata.mqpasswd)==0 || (uint8_t)udata.mqpasswd[0]==0xFF ) res = -1;
	
	return res;
}

//回复设备未注册
void send_device_not_register( void ) {
	char *jsonStr;
	if ( w_str.sendLock ) {
		w_str.sendLock = 0;//上锁
		jsonStr = cjson_reply_template( nrf_str.txBuf[1]<<8|nrf_str.txBuf[2], -2, "Device not register!" );
		nrf_str.notEmpty = 0;
		GetDataPUBLISH( w_str.txBuff, 0, 1, 0, MQTT_PUBTopic, jsonStr );
		vPortFree( jsonStr );
		wifi_tcp_send_data( 1|ENABLE_IT );
		w_str.sendLock = 1;//解锁
	}
}

//回复设备不在线
void send_device_not_online( void ) {
	char *jsonStr;
	if ( w_str.sendLock ) {
		w_str.sendLock = 0;//上锁
		jsonStr = cjson_reply_template( nrf_str.txBuf[1]<<8|nrf_str.txBuf[2], -1, "Device not online!" );
		nrf_str.notEmpty = 0;
		GetDataPUBLISH( w_str.txBuff, 0, 1, 0, MQTT_PUBTopic, jsonStr );
		vPortFree( jsonStr );
		wifi_tcp_send_data( 1|ENABLE_IT );
		w_str.sendLock = 1;//解锁
	}
}

//转发成功
void send_forward_success( void ) {
	char *jsonStr;
	if ( w_str.sendLock ) {
		w_str.sendLock = 0;//上锁
		jsonStr = cjson_reply_template( nrf_str.txBuf[1]<<8|nrf_str.txBuf[2], 0, "success" );
		nrf_str.notEmpty = 0;
		GetDataPUBLISH( w_str.txBuff, 0, 1, 0, MQTT_PUBTopic, jsonStr );
		vPortFree( jsonStr );
		wifi_tcp_send_data( 1|ENABLE_IT );
		w_str.sendLock = 1;//解锁
	}
}



