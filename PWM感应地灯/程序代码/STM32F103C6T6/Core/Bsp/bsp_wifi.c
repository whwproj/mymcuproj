#include "bsp_wifi.h"

#define ws wifi_str

WIFI_STR wifi_str;
SESSION session[3];

static int send_at_command_a_single_reply( char *cmd, char *target, int headTail, uint8_t idx );
static void wifi_smartConfig( void );
static int check_wifi_connect( void );
static int connect_wifi( void );
static void get_sntp_time( void );
static uint16_t str_to_u32 ( char* data );
static void *memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen);
static void destroy_session( uint8_t pid );
static SESSION* find_sessoin_by_pid( uint8_t pid );
static int wifi_tcp_send( char* dat, uint16_t len, uint8_t pid, xTaskHandle xHandle );
static void wait_for_dma_idle( xTaskHandle xHandle );
static int send_command_check_res( char *cmd, char *resTrue, char *resFalse );
		
void wifi_init( void ) {
	uint8_t *tp = pvPortMalloc( 512 );
	
	memset( (void*)session, 0, sizeof( SESSION )*3 );
	memset( (void*)&ws, 0, sizeof( WIFI_STR ) );
	vTaskDelay(100);
	ws.rxBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	ws.txBuff = pvPortMalloc( WIFI_TXBUFF_SIZE );
	wifi_str.isConfig = 1;
	vTaskDelay(100);
	
//	sprintf( (char*)ws.txBuff, "AT+RST\r\n" );
//	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	//开启DMA接收 开启串口空闲中断
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_IDLE );//使能空闲中断
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_TC );//使能发送完毕中断
	HAL_UART_AbortReceive( &huart2 );
	
	vTaskDelay(2000);
//	HAL_UART_Receive_DMA( &huart2, tp, 512 );//启动DMA接收
//	for ( ;; ) {
//		if ( memmem( tp, 512, "ready", strlen("ready")) != NULL ) {
//			HAL_UART_AbortReceive( &huart2 );
//			vPortFree(tp);
//			break;
//		}
//	}
	
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_ON, eSetBits );//led常亮,检测wifi
	vTaskDelay(10);
	
//	read_from_flash();
//	param_str.startHour = 20;
//	param_str.endMin = 8;
//	memcpy( param_str.wifiPasswd, "12345678", strlen("12345678") );
//	memcpy( param_str.wifiName, "Tenda_ECE950", strlen("Tenda_ECE950") );
//	write_to_flash();
	
//	while(1) {
//		ws.askConfig = 0;
//		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
//		while( !ws.askConfig ){;;} 
//		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
//		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
//		HAL_UART_Transmit( &huart1, ws.rxBuff, ws.len, portMAX_DELAY );
//	}
	send_at_command_a_single_reply("ATE0", "OK", -1, strlen("OK\r\n") );

//#define haswifi
#ifdef haswifi
	//查询连接状态和信息
	if ( check_wifi_connect() == -1 ) {
	//if ( send_at_command_a_single_reply("AT+CIPSTA?", "STATUS:2", 0, 0 ) == -1 ) {
		/*---- 配置网络 -----*/
		//读取flash,获取账号密码
		read_from_flash();
		//尝试连接wifi
		if ( connect_wifi()==-1 || param_str.wifiName[0]==0 ) {
			printf( "连接失败,开启智能配网\r\n" );
			xTaskNotify( led_taskHandle, 1U<<LED_FAST_FLASHING, eSetBits );//led快闪,smarconfig
			//连接失败,开启智能配网
			wifi_smartConfig();
		}
		
		//上电自动连接AP: AT+CWAUTOCONN=1
		send_at_command_a_single_reply("AT+CWAUTOCONN=1", "OK", -1, strlen("OK\r\n") );
	}
	while ( check_wifi_connect() == -1 ){;;}
#endif	
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_ON, eSetBits );//led常亮,配置wifi
		
	//SoftAP+Station 模式: AT+CWMODE=3
	send_at_command_a_single_reply("AT+CWMODE=3", "OK", -1, strlen("OK\r\n") );
	
	//启用多连接:	AT+CIPMUX=1
	send_at_command_a_single_reply("AT+CIPMUX=1", "OK", -1, strlen("OK\r\n") );

	//设置服务器允许建立的最大连接数:		AT+CIPSERVERMAXCONN=3
	send_at_command_a_single_reply("AT+CIPSERVERMAXCONN=3", "OK", -1, strlen("OK\r\n") );

#ifdef haswifi
	//查询sntp服务器
	send_at_command_a_single_reply("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"", "OK", -1, strlen("OK\r\n") );
	
	//获取sntp时间
	get_sntp_time();
#endif
	
	//建立TCP服务器:		AT+CIPSERVER=1,8266
	send_at_command_a_single_reply("AT+CIPSERVER=1,8266", "OK", -1, strlen("OK\r\n") );
		
	wifi_str.isConfig = 0;
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_OFF, eSetBits );//led灭,配置完成
}

static uint16_t str_to_u32 ( char* data ) {
	uint16_t size;
	for ( size=0; (*data>='0' && *data<='9'); data++ ) {
		size *= 10;
		size += (*data-'0');
	}
	return size;
}

uint32_t wifi_check_online( void ) {
	
	if ( send_command_check_res("AT+CIPMUX?", ":1", ":0") == -1 )
		return pdFALSE;
	
	if ( send_command_check_res("AT+CIPSERVER?", ":0", ":1") == -1 )
		return pdFALSE;
	
	return pdTRUE;
}

void wifi_to_reconfigure( void ) {
	//启用多连接:	AT+CIPMUX=1
	send_at_command_a_single_reply("AT+CIPMUX=1", "OK", -1, strlen("OK\r\n") );
	
	//建立TCP服务器:		AT+CIPSERVER=1,8266
	send_at_command_a_single_reply("AT+CIPSERVER=1,8266", "OK", -1, strlen("OK\r\n") );

	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
}

void wifi_parse_data( void ) {
	uint8_t pos;
	uint16_t timeout = 0;
	//uint32_t bit;
	ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	//判断类型
	if ( buffCompareToBuff( (char*)ws.rxBuff, "\r\n+IPD", 6 )!=NULL ) {
		while( ws.len < 11 ) { 
			ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx); 
		}
		ws.sesp = find_sessoin_by_pid( ws.rxBuff[7]-'0' );
		
	} else if ( strstr( (char*)ws.rxBuff, "CONNECT" )!=NULL ) {
		ws.sesp = find_sessoin_by_pid( ws.rxBuff[0]-'0' );
		wifi_tcp_send( PAGE_MENU_1, strlen(PAGE_MENU_1), ws.sesp->pid, usart_wifi_TaskHandle );
		
	}else if ( strstr( (char*)ws.rxBuff, "CLOSED" )!=NULL ) {
		destroy_session( ws.rxBuff[0]-'0' );
		
	} 

	if( strstr( (char*)ws.rxBuff, "+IPD" ) == NULL ){
		HAL_UART_AbortReceive( &huart2 );
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
		memset( ws.rxBuff, 0, WIFI_BUFF_SIZE );
		return;
	}
	ws.dLen = str_to_u32( (char*)(ws.rxBuff+9) );//\r\n+IPD,0,
	pos = 11;//\r\n+IPD,0,x:
	if ( ws.dLen>=10 ) { pos++; }
	if ( ws.dLen>=100 ) { pos++; }
	while( (ws.len!=ws.dLen+pos) && (timeout<50) ) {
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &hdma_usart2_rx );
		vTaskDelay(10);
		timeout++;
	}
	memcpy( ws.sesp->data, &ws.rxBuff[pos], ws.dLen );
	ws.sesp->dataLen = ws.dLen;
	memset( ws.rxBuff, 0, WIFI_BUFF_SIZE );
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	if ( timeout == 50 ) { return; }
	//xTaskNotify( usart_wifi_TaskHandle, pdFALSE, eSetBits );
	//xTaskNotifyWait( pdFALSE, ~(1U<<WIFI_SEND_OK), &bit, portMAX_DELAY );
	
//	sprintf( (char*)debug_str.txBuff, "接收到 pid_%d 的消息: ", session.pid );
//	HAL_UART_Transmit( &huart1,debug_str.txBuff, strlen((char*)debug_str.txBuff), 0xffff );
//	HAL_UART_Transmit( &huart1, ws.rxBuff+pos, ws.dLen, 0xffff );
//	printf("\r\n");
	
	//接收到wifi通信数据,发送wifi正在通信的通知
	xTaskNotify( check_online_taskHandle, 1U<<WIFI_IS_COMMUNICATION, eSetBits );
	
	xQueueSend( cmd_queueHandle, &ws.sesp, pdFALSE );
	if ( ws.sesp->dir == 0 ) {//main
		xTaskNotify( cmd_handle_taskHandle, 1U<<CMD_MAIN, eSetBits );
	} else if ( ws.sesp->dir == 1 ) {
		xTaskNotify( cmd_handle_taskHandle, 1U<<CMD_SUB_1, eSetBits );
	} else if ( ws.sesp->dir == 2 ) {
		xTaskNotify( cmd_handle_taskHandle, 1U<<CMD_SUB_2, eSetBits );
	} else if ( ws.sesp->dir == 3 ) {
		xTaskNotify( cmd_handle_taskHandle, 1U<<CMD_SUB_3, eSetBits );
	}
	
}

static int wifi_tcp_send( char* dat, uint16_t len, uint8_t pid, xTaskHandle xHandle ) {
	uint16_t num;
	int res = 0;
	
	ws.passDataStop = 1;//停止发送解析通知
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	wait_for_dma_idle( xHandle );//等待DMA空闲
	sprintf( (char*)ws.txBuff, "AT+CIPSEND=%d,%d\r\n", pid, len );
	//HAL_UART_Transmit(&huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	HAL_UART_Transmit_DMA( &huart2, ws.txBuff, strlen((char*)ws.txBuff) );
	for ( num=300; num>0; num-- ) {
		vTaskDelay(10);
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &hdma_usart2_rx );
		if ( ws.rxBuff[ws.len-1] == '>' )
			break;
	}
	if ( num == 0 ) {
		//发送异常
		res = -1;
		goto ret;
	}
	
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	wait_for_dma_idle( xHandle );//等待DMA空闲
	sprintf( (char*)ws.txBuff, "%s", dat );
	HAL_UART_Transmit_DMA( &huart2, ws.txBuff, len );
	for ( num=300; num>0; num-- ) {
		vTaskDelay(10);
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &hdma_usart2_rx );
		if ( buffCompareToBuff( "SEND OK\r\n", (char*)&ws.rxBuff[ws.len-9], 9 ) )
			break;
	}
	if ( num == 0 ) {
		//发送异常
		res = -1;
	}
	
ret:
	ws.passDataStop = 0;//取消暂停发送解析通知
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	return res;
}

static void wait_for_dma_idle( xTaskHandle xHandle ) {
	uint32_t newBits;
	uint8_t num;
	//----等待DMA发送空闲 start -----
	xTaskNotify( xHandle, pdFALSE, eSetBits );
	do {
		//允许发送的通知,首次触发和DMA发送完成后触发
		xTaskNotifyWait( pdFALSE, (1U<<WIFI_SEND_OK), &newBits, 700 );
	} while( !(newBits & (1U<<WIFI_SEND_OK)) && ++num<4 );
	//----等待DMA发送空闲 end -----
}

static int send_at_command_a_single_reply( char *cmd, char *target, 
	int headTail, uint8_t idx ) {
	uint8_t times = 5;
	ws.askConfig = 0;
	sprintf( (char*)ws.txBuff, "%s\r\n", cmd );
	while ( times-- ) { 
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启用DMA接收
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;} 
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		
		if ( headTail == 0 ) {//0:head
			if ( !buffCompareToBuff( target, (char *)&ws.rxBuff[idx], strlen(target) ) ){
				//return -1;
				vTaskDelay(500);
			} else {
				return 0;
			}
		} else if ( headTail == -1 ) {//-1:tail
			if ( !buffCompareToBuff( target, (char *)&ws.rxBuff[ws.len-idx], strlen(target) ) ){
				//return -1;
				vTaskDelay(500);
			} else {
				return 0;
			}
		}
	}
	return -1;
}
static int send_command_check_res( char *cmd, char *resTrue, char *resFalse ) {
	uint8_t times = 5;
	ws.askConfig = 0;
	sprintf( (char*)ws.txBuff, "%s\r\n", cmd );
	
	while ( times-- ) { 
		HAL_UART_AbortReceive( &huart2 );
		memset( ws.rxBuff, 0, WIFI_BUFF_SIZE );
		vTaskDelay(30);
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启用DMA接收
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;} 
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		if ( strstr((char*)ws.rxBuff, resTrue) != NULL ) {
			return 0;
		} else if ( strstr((char*)ws.rxBuff, resFalse) != NULL ) {
			return -1;
		}
		vTaskDelay(500);
	}
	return -1;
}

static void wifi_smartConfig( void ) {
	char *ppos;
	uint8_t i;
	uint16_t cont;
	
	taskENTER_CRITICAL();
	memset(wifi_str.rxBuff, 0, DEBUG_BUFF_SIZE);
	vTaskDelay(100);
	printf( "\r\n------\r\n启用智能配网\r\n------\r\n" );
	taskEXIT_CRITICAL();
	//配网start(灯闪烁)
	send_at_command_a_single_reply("AT+CWMODE=1", "OK", -1, strlen("OK\r\n") );
	//智能配网:  AT+CWSTARTSMART=3
	
	for( ;;	) {
		cont = 250;//25s
		ws.askConfig = 0;
		sprintf((char*)ws.txBuff, "AT+CWSTOPSMART\r\n");
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;}
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.askConfig = 0;
		sprintf((char*)ws.txBuff, "AT+CWSTARTSMART=3\r\n");
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;} 
		vTaskDelay(50);
		//ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( !buffCompareToBuff( "\r\nOK\r\n", (char*)ws.rxBuff, 6 ) ) {
			vTaskDelay(100);
			HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
			HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
			continue;
		}
		while ( strstr((char*)ws.rxBuff,"smartconfig") == NULL ){;;}
		while ( cont ) {
			ws.askConfig = 0;
			while( !ws.askConfig ){ 
				vTaskDelay(100); --cont;
				if ( cont == 0 ) break;
			}
			if ( cont == 0 ) break;
			//ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
			if ( strstr((char*)ws.rxBuff,"WIFI CONNECTED") != NULL ) {
				HAL_UART_AbortReceive( &huart2 );
				memset( param_str.wifiName, 0, 50 );
				memset( param_str.wifiPasswd, 0, 30 );
				vTaskDelay(50);
				ppos = strstr((char*)ws.rxBuff,"ssid:")+5;
				i=0; while( ppos[i]!='\r' ){i++;}
				memcpy( param_str.wifiName, ppos, i);
				ppos = strstr((char*)ws.rxBuff,"password:")+9;
				i=0; while( ppos[i]!='\r' ){i++;}
				memcpy( param_str.wifiPasswd, ppos, i);
				cont = 1;
				write_to_flash();//存储账号密码
				break;
			}
		}
		printf("\r\n----- \r\n%s\r\n", ws.rxBuff);
		if ( cont == 0 ) {
			xTaskNotify( led_taskHandle, 1U<<LED_SLOW_FLASHING, eSetBits );//led慢闪,连接wifi
			vTaskDelay(10);
			continue;//超时未连接,密码错误,重新开启
		}
		break;
	}
	//退出智能配网:   AT+CWSTOPSMART
	send_at_command_a_single_reply("AT+CWSTOPSMART", "OK", -1, strlen("OK\r\n") );

	//配网end(灯关闭)
}


static int check_wifi_connect() {
	uint8_t timeout = 5;
	sprintf( (char*)ws.txBuff, "AT+CIPSTA?\r\n" );	
	while( timeout ) {	
		ws.askConfig = 0;
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;}
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( buffCompareToBuff( "OK\r\n", (char *)&ws.rxBuff[ws.len - strlen("OK\r\n")], strlen("OK\r\n") ) ) {
			if ( !buffCompareToBuff( "\"0.0.0.0\"", (char *)&ws.rxBuff[ws.len - strlen("\"0.0.0.0\"\r\n\r\nOK\r\n")], strlen("\"0.0.0.0\"") ) ) {
				return 0;
			} else {
				return -1;
			}
		} else {
			timeout--;
		}
		vTaskDelay( 500 );
	}
	return -1;
}

static int connect_wifi( void ) {
	uint8_t timeout = 10;
	sprintf( (char*)ws.txBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", param_str.wifiName, param_str.wifiPasswd );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );
	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	while( timeout-- ) {	
		ws.askConfig = 0;
		while( !ws.askConfig ){;;}
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( buffCompareToBuff( "ERROR\r\n", (char *)&ws.rxBuff[ws.len - strlen("ERROR\r\n")], strlen("ERROR\r\n") ) ) {
			printf( "wifi连接失败!\r\n" );
			return -1;
		} else if ( buffCompareToBuff( "OK\r\n", (char *)&ws.rxBuff[ws.len - strlen("OK\r\n")], strlen("OK\r\n") ) ) {
			printf( "wifi连接成功!\r\n" );
			return 0;
		} else {
			HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//重启DMA接收
		}
	}
	return -1;
}


static void get_sntp_time( void ) {
	#define ps param_str
	uint8_t times = 10;
	sprintf( (char*)ws.txBuff, "AT+CIPSNTPTIME?\r\n" );
	while(times) {
		ws.askConfig = 0;
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen("AT+CIPSNTPTIME?\r\n"), portMAX_DELAY );
		while( !ws.askConfig ){;;}
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		//printf("---\r\n%s\r\n---\r\n", ws.rxBuff);
		if ( buffCompareToBuff( "OK\r\n", (char*)&ws.rxBuff[ws.len-strlen("OK\r\n")], strlen("OK\r\n") ) ) {
			if ( !buffCompareToBuff( "1970", (char*)&ws.rxBuff[ws.len-strlen("1970\r\nOK\r\n")], strlen("1970") ) ) {
				//读取到更新的时间
				ps.nowHour = (ws.rxBuff[ws.len-strlen("hh:mm:ss yyyy\r\nOK\r\n")]-0x30)*10 + ws.rxBuff[ws.len-strlen("h:mm:ss yyyy\r\nOK\r\n")]-0x30;
				ps.nowMin = (ws.rxBuff[ws.len-strlen("mm:ss yyyy\r\nOK\r\n")]-0x30)*10 + ws.rxBuff[ws.len-strlen("m:ss yyyy\r\nOK\r\n")]-0x30;
				check_turn_on_time();
				break;
			} else {
				times--;
			}
		}
		vTaskDelay( 1000 );
	}
}


int check_turn_on_time( void ) {
	ps.startHour = 19;
	ps.endHour = 20;
	if ( ps.endHour > ps.startHour ) {//一天当中
		if ( ps.nowHour>=ps.startHour && ps.nowHour<ps.endHour ) {
			ps.runTime = ( ps.endHour - (ps.nowHour+1) )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			return 0;
		} else {
			return -1; 
		}
	} else {//终止时间后一天
		if ( ( ps.nowHour>=ps.startHour && ps.nowHour<=23 ) || ( ps.nowHour<ps.endHour ) ) {
			if ( ps.nowHour>=ps.startHour && ps.nowHour<=23 ) {
				ps.runTime = ( 24 - (ps.nowHour+1) + ps.endHour )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			} else {
				ps.runTime = ( ps.endHour - (ps.nowHour+1) )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			}
			//计算关机时间,发送通知
			return 0;
		} else { 
			return -1; 
		}
	}
}

static void *memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen) {
    const unsigned char *h = haystack;
    const unsigned char *n = needle;
    size_t i, j;
    for (i = 0; i <= haystacklen - needlelen; i++) {
        for (j = 0; j < needlelen; j++) {
            if (h[i + j] != n[j]) {
                break;
            }
        }
        if (j == needlelen) {
            return (void *)&h[i];
        }
    }
    return NULL;
}


static void destroy_session( uint8_t pid ) {
	uint8_t i;
	for( i=0; i<3; i++ ) {
		if ( session[i].onlineSta && session[i].pid==pid ) {
			session[i].onlineSta = 0;
			session[i].dir = 0;
			return;
		}
	}
}
static SESSION* find_sessoin_by_pid( uint8_t pid ) {
	uint8_t i;
	for( i=0; i<3; i++ ) {
		if ( session[i].onlineSta && session[i].pid==pid ) {
			return &session[i];
		}
	}
	for( i=0; i<3; i++ ) {
		if ( !session[i].onlineSta ) {
			session[i].onlineSta = 1;
			session[i].pid = pid;
			return &session[i];
		}
	}
	return NULL;
}


uint8_t cmd_main_fun( void ) {
	SESSION *sessp;
	uint8_t num;
	if ( xQueueReceive( cmd_queueHandle, &sessp, 10 ) ) {
		if ( buffCompareToBuff( "1", sessp->data, 1 ) ) {
			xTaskNotify( pwm_taskHandle, 1U<<TURN_ON, eSetBits );
			wifi_tcp_send( "已发送开灯指令\r\n", strlen("已发送开灯指令\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "2", sessp->data, 1 ) ) {
			xTaskNotify( pwm_taskHandle, 1U<<TURN_OFF, eSetBits );
			wifi_tcp_send( "已发送关灯指令\r\n", strlen("已发送关灯指令\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "3", sessp->data, 1 ) ) {
			xTaskNotify( pwm_taskHandle, 1U<<FLASHING, eSetBits );
			wifi_tcp_send( "已发送闪烁指令\r\n", strlen("已发送闪烁指令\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "4", sessp->data, 1 ) ) {
			wifi_tcp_send( "功能待开发...\r\n", strlen("功能待开发...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "5", sessp->data, 1 ) ) {
			wifi_tcp_send( PAGE_MENU_2, strlen(PAGE_MENU_2), sessp->pid, cmd_handle_taskHandle );
			ws.sesp->dir = 1;
		}
		else {
			wifi_tcp_send( PAGE_MENU_1, strlen(PAGE_MENU_1), sessp->pid, cmd_handle_taskHandle );
		}
	}
	num = uxQueueMessagesWaiting( cmd_queueHandle );
	return num;
}
uint8_t cmd_sub_1_fun( void ) {//参数设置
	SESSION *sessp;
	uint8_t num;
	//二.参数设置页:\r\n1.设置天黑时间\r\n2.设置天亮时间\r\n3.调节最高亮度\r\n按#号返回上一层菜单
	if ( xQueueReceive( cmd_queueHandle, &sessp, 10 ) ) {
		if ( buffCompareToBuff( "1", sessp->data, 1 ) ) {
			wifi_tcp_send( "功能待开发...\r\n", strlen("功能待开发...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "2", sessp->data, 1 ) ) {
			wifi_tcp_send( "功能待开发...\r\n", strlen("功能待开发...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "3", sessp->data, 1 ) ) {
			wifi_tcp_send( PAGE_MENU_3, strlen(PAGE_MENU_3), sessp->pid, cmd_handle_taskHandle );
			ws.sesp->dir = 2;
		}
		else if ( buffCompareToBuff( "#", sessp->data, 1 ) || buffCompareToBuff( "＃", sessp->data, 1 ) ) {
			wifi_tcp_send( PAGE_MENU_1, strlen(PAGE_MENU_1), sessp->pid, cmd_handle_taskHandle );
			ws.sesp->dir = 0;
		}
		else {
			wifi_tcp_send( PAGE_MENU_2, strlen(PAGE_MENU_2), sessp->pid, cmd_handle_taskHandle );
		}
	}
	num = uxQueueMessagesWaiting( cmd_queueHandle );
	return num;
}
uint8_t cmd_sub_2_fun( void ) {
	SESSION *sessp;
	uint8_t num, t;
	char *str;
	//三.调节最高亮度页:\r\n请输入数字调节最高亮度百分比 20 - 100 之间\r\n按下#号键返回上一层菜单
	if ( xQueueReceive( cmd_queueHandle, &sessp, 10 ) ) {
		sessp->data[ sessp->dataLen ] = 0;
		t = ( uint8_t )str_to_u32( sessp->data );
		if ( t >= 20 && t <= 100 ) {
			pwm_str.maxBtnPer = t;
			str = pvPortMalloc( 100 );
			sprintf( str, "成功设置最大亮度为: %d%%", pwm_str.maxBtnPer );
			wifi_tcp_send( str, strlen(str), sessp->pid, cmd_handle_taskHandle );
			vPortFree( str );
			wifi_tcp_send( "三.调节最高亮度页:\r\n请输入数字调节最高亮度百分比 20 - 100 之间\r\n按下#号键返回上一层菜单", strlen("三.调节最高亮度页:\r\n请输入数字调节最高亮度百分比 20 - 100 之间\r\n按下#号键返回上一层菜单"), sessp->pid, cmd_handle_taskHandle );
		} 
		else if ( buffCompareToBuff( "#", sessp->data, 1 ) || buffCompareToBuff( "＃", sessp->data, 1 ) ) {
			wifi_tcp_send( "二.参数设置页:\r\n1.设置天黑时间\r\n2.设置天亮时间\r\n3.调节最高亮度\r\n按#号返回上一层菜单", strlen("二.参数设置页:\r\n1.设置天黑时间\r\n2.设置天亮时间\r\n3.调节最高亮度\r\n按#号返回上一层菜单"), sessp->pid, cmd_handle_taskHandle );
			ws.sesp->dir = 1;
		}
		else {
			wifi_tcp_send( "三.调节最高亮度页:\r\n请输入数字调节最高亮度百分比 20 - 100 之间\r\n按下#号键返回上一层菜单", strlen("三.调节最高亮度页:\r\n请输入数字调节最高亮度百分比 20 - 100 之间\r\n按下#号键返回上一层菜单"), sessp->pid, cmd_handle_taskHandle );
		}
	}
	num = uxQueueMessagesWaiting( cmd_queueHandle );
	return num;
}
uint8_t cmd_sub_3_fun( void ) {
	SESSION *sessp;
	uint8_t num;
	if ( xQueueReceive( cmd_queueHandle, &sessp, 10 ) ) {
		if ( buffCompareToBuff( "1", sessp->data, 1 ) ) {
			wifi_tcp_send( "功能待开发...\r\n", strlen("功能待开发...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		if ( buffCompareToBuff( "2", sessp->data, 1 ) ) {
			wifi_tcp_send( "功能待开发...\r\n", strlen("功能待开发...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		if ( buffCompareToBuff( "3", sessp->data, 1 ) ) {
			wifi_tcp_send( "功能待开发...\r\n", strlen("功能待开发...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
	}
	num = uxQueueMessagesWaiting( cmd_queueHandle );
	return num;
}







