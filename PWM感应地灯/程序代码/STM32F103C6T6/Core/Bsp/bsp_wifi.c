#include "bsp_wifi.h"

#define ws wifi_str

WIFI_STR wifi_str;

static int send_at_command_a_single_reply( char *cmd, char *target, int headTail, uint8_t idx );
static void wifi_smartConfig( void );
static int check_wifi_connect( void );
static int connect_wifi( void );
static void get_sntp_time( void );

		
void wifi_init( void ) {
	
	memset( (void*)&ws, 0, sizeof( WIFI_STR ) );
	vTaskDelay(100);
	ws.rxBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	ws.txBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	
	//开启DMA接收 开启串口空闲中断
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_IDLE );//使能空闲中断
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_TC );//使能发送完毕中断
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	
	wifi_str.isConfig = 1;
	
//	while(1) {
//		ws.askConfig = 0;
//		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
//		while( !ws.askConfig ){;;} 
//		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
//		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
//		HAL_UART_Transmit( &huart1, ws.rxBuff, ws.len, portMAX_DELAY );
//	}
	send_at_command_a_single_reply("ATE0", "OK", -1, strlen("OK\r\n") );
	
	//查询连接状态和信息
	if ( check_wifi_connect() == -1 ) {
	//if ( send_at_command_a_single_reply("AT+CIPSTA?", "STATUS:2", 0, 0 ) == -1 ) {
		/*---- 配置网络 -----*/
		//读取flash,获取账号密码
		read_from_flash();
		//尝试连接wifi
		if ( connect_wifi() == -1 ) {
			//连接失败,开启智能配网
			wifi_smartConfig();
		}
		
		//上电自动连接AP: AT+CWAUTOCONN=1
		send_at_command_a_single_reply("AT+CWAUTOCONN=1", "OK", -1, strlen("OK\r\n") );
	}
	while ( check_wifi_connect() == -1 ){;;}
		
	//SoftAP+Station 模式: AT+CWMODE=3
	send_at_command_a_single_reply("AT+CWMODE=3", "OK", -1, strlen("OK\r\n") );
	
	//启用多连接:	AT+CIPMUX=1
	send_at_command_a_single_reply("AT+CIPMUX=1", "OK", -1, strlen("OK\r\n") );

	//设置服务器允许建立的最大连接数:		AT+CIPSERVERMAXCONN=2
	send_at_command_a_single_reply("AT+CIPSERVERMAXCONN=2", "OK", -1, strlen("OK\r\n") );
	
	//查询sntp服务器
	send_at_command_a_single_reply("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"", "OK", -1, strlen("OK\r\n") );
	
	//获取sntp时间
	get_sntp_time();
	
	//建立TCP服务器:		AT+CIPSERVER=1,8266
	send_at_command_a_single_reply("AT+CIPSERVER=1,8266", "OK", -1, strlen("OK\r\n") );
		
	//res = send_at_command_a_single_reply("AT+CIPSTATUS", "STATUS:2", 0, 0 );
	
	//printf("res: %d\r\n", res);
	while(1) {
		ws.askConfig = 0;
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
		while( !ws.askConfig ){;;} 
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		HAL_UART_Transmit( &huart1, ws.rxBuff, ws.len, portMAX_DELAY );
	}
	
	//wifi连接检测中 led闪烁_start
	//vTaskDelay( 3000 );

	//AT+CWJAP="MI","12345678"
}


void wifi_parse_data( void ) {
	ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	//判断类型
}

static int send_at_command_a_single_reply( char *cmd, char *target, 
	int headTail, uint8_t idx ) {
	uint8_t times = 5;
	ws.askConfig = 0;
	sprintf( (char*)ws.txBuff, "%s\r\n", cmd );
	while ( times-- ) { 
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
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

static void wifi_smartConfig( void ) {
	printf( "启用智能配网\r\n" );
	//配网start(灯闪烁)
	send_at_command_a_single_reply("AT+CWMODE=1", "OK", -1, strlen("OK\r\n") );
	//智能配网:  AT+CWSTARTSMART=3
	sprintf(ws.txBuff, "AT+CWSTARTSMART=3\r\n");
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	for( ;;	) {
		ws.askConfig = 0;
		while( !ws.askConfig ){;;} 
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( buffCompareToBuff("") )
	}
	
	
	
	
	
//	sprintf( param_str.wifiName, "MI");
//	sprintf( param_str.wifiPasswd, "12345678");
	write_to_flash();
	
	
	
	//退出智能配网:   AT+CWSTOPSMART
	
	//配网end(灯关闭)
}


static int check_wifi_connect() {
	uint8_t timeout = 10;
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
		vTaskDelay( 1000 );
	}
	return -1;
}

static int connect_wifi( void ) {
	uint8_t timeout = 10;
	sprintf( (char*)ws.txBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", param_str.wifiName, param_str.wifiPasswd );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );
	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	while( timeout ) {	
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
			timeout--;
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
				ws.nowHour = (ws.rxBuff[ws.len-strlen("hh:mm:ss yyyy\r\nOK\r\n")]-0x30)*10 + ws.rxBuff[ws.len-strlen("h:mm:ss yyyy\r\nOK\r\n")]-0x30;
				ws.nowMin = (ws.rxBuff[ws.len-strlen("mm:ss yyyy\r\nOK\r\n")]-0x30)*10 + ws.rxBuff[ws.len-strlen("m:ss yyyy\r\nOK\r\n")]-0x30;
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
	ws.startHour = 19;
	ws.endHour = 20;
	if ( ws.endHour > ws.startHour ) {//一天当中
		if ( ws.nowHour>=ws.startHour && ws.nowHour<ws.endHour ) {
			ws.runTime = ( ws.endHour - (ws.nowHour+1) )*3600000 + (60-ws.nowMin)*60000 | UNTIL_TIME;
			return 0;
		} else {
			return -1; 
		}
	} else {//终止时间后一天
		if ( ( ws.nowHour>=ws.startHour && ws.nowHour<=23 ) || ( ws.nowHour<ws.endHour ) ) {
			if ( ws.nowHour>=ws.startHour && ws.nowHour<=23 ) {
				ws.runTime = ( 24 - (ws.nowHour+1) + ws.endHour )*3600000 + (60-ws.nowMin)*60000 | UNTIL_TIME;
			} else {
				ws.runTime = ( ws.endHour - (ws.nowHour+1) )*3600000 + (60-ws.nowMin)*60000 | UNTIL_TIME;
			}
			//计算关机时间,发送通知
			return 0;
		} else { 
			return -1; 
		}
	}
	
	
	
	
}








