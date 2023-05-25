#include "bsp_wifi.h"

#define ws wifi_str

WIFI_STR wifi_str;

static int send_at_command_a_single_reply( char *cmd, char *target, int headTail, uint8_t idx );
static void wifi_smartConfig( void );
		
void wifi_init( void ) {
	int res;
	
	memset( (void*)&ws, 0, sizeof( WIFI_STR ) );
	ws.rxBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	ws.txBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	//开启DMA接收 开启串口空闲中断
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_IDLE );//使能空闲中断
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_TC );//使能发送完毕中断
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	
	wifi_str.isConfig = 1;
	
	//查询连接状态和信息
	if ( send_at_command_a_single_reply("AT+CIPSTATUS", "STATUS:2", 0, 0 ) == -1 ) {
		//配置网络
		//Station 模式:   AT+CWMODE=1
		send_at_command_a_single_reply("AT+CWMODE=1", "OK", 0, 0 );
		
		//读取flash,获取账号密码
		if (  ) {//连接失败,再进入配网模式
			
		}
		
		//智能配网
		wifi_smartConfig();
		
		//上电自动连接AP: AT+CWAUTOCONN=1
		send_at_command_a_single_reply("AT+CWMODE=1", "OK", 0, 0 );
		
		//启用多连接:			AT+CIPMUX=1
		send_at_command_a_single_reply("AT+CWMODE=1", "OK", 0, 0 );
		
		//SoftAP+Station 模式: 
		send_at_command_a_single_reply("AT+CWMODE=1", "OK", 0, 0 );
		
	}
	
	res = send_at_command_a_single_reply("AT+CIPSTATUS", "STATUS:2", 0, 0 );
	
	printf("res: %d\r\n", res);
	while(1) {
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
		while( !ws.askConfig ){;;} 
		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		HAL_UART_Transmit( &huart1, ws.rxBuff, ws.len, portMAX_DELAY );
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//启动DMA接收
	}
	
	//wifi连接检测中 led闪烁_start
	vTaskDelay( 3000 );
	

	//AT+CWJAP="MI","12345678"
	
	
	 
	
}


void wifi_parse_data( void ) {
	ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	//判断类型
}

static int send_at_command_a_single_reply( char *cmd, char *target, 
	int headTail, uint8_t idx ) {
	ws.askConfig = 0;
	sprintf( (char*)ws.txBuff, "%s\r\n", cmd );
	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	while( !ws.askConfig ){;;} 
	vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
	ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	
	if ( headTail == 0 ) {//0:head
		if ( !buffCompareToBuff( target, (char *)&ws.rxBuff[idx], strlen(target) ) ){
			return -1;
		} else {
			return 0;
		}
	} else if ( headTail == -1 ) {//-1:tail
		if ( !buffCompareToBuff( target, (char *)&ws.rxBuff[ws.len-idx], strlen(target) ) ){
			return -1;
		} else {
			return 0;
		}
	}
	return -1;
}

static void wifi_smartConfig( void ) {
	//配网start(灯闪烁)
	//智能配网:       AT+CWSTARTSMART=3
	//退出智能配网:   AT+CWSTOPSMART
	
	//配网end(灯关闭)
}




