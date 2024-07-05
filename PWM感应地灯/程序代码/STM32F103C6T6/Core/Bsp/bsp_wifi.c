#include "bsp_wifi.h"

#define ws wifi_str

WIFI_STR wifi_str;
SESSION session[2];

static int send_at_command_a_single_reply( char *cmd, char *target );
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
static int send_command_check_res( char *cmd, char *resTrue );
static void reset_rx_dma( void );
		
void wifi_init( void ) {
	uint8_t *tp = pvPortMalloc( 512 );
	
	memset( (void*)session, 0, sizeof( SESSION )*2 );
	memset( (void*)&ws, 0, sizeof( WIFI_STR ) );
	vTaskDelay(100);
	ws.rxBuff = pvPortMalloc( WIFI_BUFF_SIZE );
	ws.txBuff = pvPortMalloc( WIFI_TXBUFF_SIZE );
	wifi_str.isConfig = 1;
	vTaskDelay(100);
	
//	sprintf( (char*)ws.txBuff, "AT+RST\r\n" );
//	HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
	//����DMA���� �������ڿ����ж�
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_IDLE );//ʹ�ܿ����ж�
	__HAL_UART_ENABLE_IT( &huart2, UART_IT_TC );//ʹ�ܷ�������ж�
	HAL_UART_AbortReceive( &huart2 );
	
	vTaskDelay(2000);
//	HAL_UART_Receive_DMA( &huart2, tp, 512 );//����DMA����
//	for ( ;; ) {
//		if ( memmem( tp, 512, "ready", strlen("ready")) != NULL ) {
//			HAL_UART_AbortReceive( &huart2 );
//			vPortFree(tp);
//			break;
//		}
//	}
	
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_ON, eSetBits );//led����,���wifi
	vTaskDelay(10);
	
//	read_from_flash();
//	param_str.startHour = 20;
//	param_str.endMin = 8;
//	memcpy( param_str.wifiPasswd, "12345678", strlen("12345678") );
//	memcpy( param_str.wifiName, "Tenda_ECE950", strlen("Tenda_ECE950") );
//	write_to_flash();
	
//	while(1) {
//		ws.askConfig = 0;
//		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
//		while( !ws.askConfig ){;;} 
//		vTaskDelay(50);HAL_UART_AbortReceive( &huart2 );
//		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
//		HAL_UART_Transmit( &huart1, ws.rxBuff, ws.len, portMAX_DELAY );
//	}
	send_at_command_a_single_reply("ATE0", "OK" );
	
	
	send_at_command_a_single_reply("AT+CIPSTO=120", "OK" );
	
	

//#define haswifi
#ifdef haswifi
	//��ѯ����״̬����Ϣ
	if ( check_wifi_connect() == -1 ) {
	//if ( send_at_command_a_single_reply("AT+CIPSTA?", "STATUS:2" ) {
		/*---- �������� -----*/
		//��ȡflash,��ȡ�˺�����
		read_from_flash();
		//��������wifi
		if ( connect_wifi()==-1 || param_str.wifiName[0]==0 ) {
			printf( "����ʧ��,������������\r\n" );
			xTaskNotify( led_taskHandle, 1U<<LED_FAST_FLASHING, eSetBits );//led����,smarconfig
			//����ʧ��,������������
			wifi_smartConfig();
		}
		
		//�ϵ��Զ�����AP: AT+CWAUTOCONN=1
		send_at_command_a_single_reply("AT+CWAUTOCONN=1", "OK" );
	}
	while ( check_wifi_connect() == -1 ){;;}
#endif	
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_ON, eSetBits );//led����,����wifi
	vTaskDelay(10);
		
	//SoftAP+Station ģʽ: AT+CWMODE=3
	send_at_command_a_single_reply("AT+CWMODE=3", "OK" );
	
	//���ö�����:	AT+CIPMUX=1
	send_at_command_a_single_reply("AT+CIPMUX=1", "OK" );

	//���÷����������������������:		AT+CIPSERVERMAXCONN=3
	send_at_command_a_single_reply("AT+CIPSERVERMAXCONN=3", "OK" );

#ifdef haswifi
	//��ѯsntp������
	send_at_command_a_single_reply("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"", "OK" );
	
	//��ȡsntpʱ��
	get_sntp_time();
#endif
	
	//����TCP������:		AT+CIPSERVER=1,8266
	send_at_command_a_single_reply("AT+CIPSERVER=1,8266", "OK" );
		
	wifi_str.isConfig = 0;
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	xTaskNotify( led_taskHandle, 1U<<LED_TURN_OFF, eSetBits );//led��,�������
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
	uint32_t res = pdTRUE;
	
	if ( ws.checkOnlineNum++ > 100 ) {
		ws.checkOnlineNum = 0;
		HAL_UART_AbortReceive( &huart2 );
		sprintf( (char*)ws.txBuff, "AT+RST\r\n" );
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		HAL_Delay(2500);
	}
	
	if ( send_command_check_res("AT+CIPMUX?", ":1" ) == -1 )
		res = pdFALSE;
	
	vTaskDelay(500);
	
	if ( send_command_check_res("AT+CIPSERVER?", ":0") != -1 )
		res = pdFALSE;
	
	vTaskDelay(500);
	
	reset_rx_dma();

	return res;
}

void wifi_to_reconfigure( void ) {
	
	//���ö�����:	AT+CIPMUX=1
	send_at_command_a_single_reply("AT+CIPMUX=1", "OK" );
	
	//����TCP������:		AT+CIPSERVER=1,8266
	send_at_command_a_single_reply("AT+CIPSERVER=1,8266", "OK" );

	reset_rx_dma();
}

void wifi_parse_data( void ) {
	char *prbuff;
	uint8_t pos;
	uint16_t timeout = 0;
	//uint32_t bit;
	ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	//�ж�����
	//if ( buffCompareToBuff( (char*)ws.rxBuff, "\r\n+IPD", 6 )!=NULL ) {
	
	if ( strstr( (char*)ws.rxBuff, "\r\n+IPD" ) != NULL ) {
		prbuff = strstr( (char*)ws.rxBuff, "\r\n+IPD," )+7;
		ws.sesp = find_sessoin_by_pid( prbuff[0]-'0' );
		//���յ�wifiͨ������,����wifi����ͨ�ŵ�֪ͨ
		xTaskNotify( check_online_taskHandle, 1U<<WIFI_IS_COMMUNICATION, eSetBits );
		
	} else if ( strstr( (char*)ws.rxBuff, ",CONNECT" )!=NULL ) {
		prbuff = strstr( (char*)ws.rxBuff, ",CONNECT" )-1;
		//���յ�wifiͨ������,����wifi����ͨ�ŵ�֪ͨ
		xTaskNotify( check_online_taskHandle, 1U<<WIFI_IS_COMMUNICATION, eSetBits );
		ws.sesp = find_sessoin_by_pid( prbuff[0]-'0' );
		wifi_tcp_send( PAGE_MENU_1, strlen(PAGE_MENU_1), ws.sesp->pid, usart_wifi_TaskHandle );
		
	}else if ( strstr( (char*)ws.rxBuff, "CLOSED" )!=NULL ) {
		prbuff = strstr( (char*)ws.rxBuff, ",CLOSED" )-1;
		destroy_session( prbuff[0]-'0' );
	} 

	if( strstr( (char*)ws.rxBuff, "+IPD" )==NULL || ws.sesp==NULL){
		HAL_UART_AbortReceive( &huart2 );
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
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
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	if ( timeout == 50 ) { return; }
	//xTaskNotify( usart_wifi_TaskHandle, pdFALSE, eSetBits );
	//xTaskNotifyWait( pdFALSE, ~(1U<<WIFI_SEND_OK), &bit, portMAX_DELAY );
	
//	sprintf( (char*)debug_str.txBuff, "���յ� pid_%d ����Ϣ: ", session.pid );
//	HAL_UART_Transmit( &huart1,debug_str.txBuff, strlen((char*)debug_str.txBuff), 0xffff );
//	HAL_UART_Transmit( &huart1, ws.rxBuff+pos, ws.dLen, 0xffff );
//	printf("\r\n");
	
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
	
	ws.passDataStop = 1;//ֹͣ���ͽ���֪ͨ
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	wait_for_dma_idle( xHandle );//�ȴ�DMA����
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
		//�����쳣
		res = -1;
		goto ret;
	}
	
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	wait_for_dma_idle( xHandle );//�ȴ�DMA����
	sprintf( (char*)ws.txBuff, "%s", dat );
	HAL_UART_Transmit_DMA( &huart2, ws.txBuff, len );
	for ( num=300; num>0; num-- ) {
		vTaskDelay(10);
		ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &hdma_usart2_rx );
		if ( buffCompareToBuff( "SEND OK\r\n", (char*)&ws.rxBuff[ws.len-9], 9 ) )
			break;
	}
	if ( num == 0 ) {
		//�����쳣
		res = -1;
	}
	
ret:
	ws.passDataStop = 0;//ȡ����ͣ���ͽ���֪ͨ
	HAL_UART_AbortReceive( &huart2 );
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
	return res;
}

static void wait_for_dma_idle( xTaskHandle xHandle ) {
	uint32_t newBits;
	uint8_t num;
	//----�ȴ�DMA���Ϳ��� start -----
	xTaskNotify( xHandle, pdFALSE, eSetBits );
	do {
		//�����͵�֪ͨ,�״δ�����DMA������ɺ󴥷�
		xTaskNotifyWait( pdFALSE, (1U<<WIFI_SEND_OK), &newBits, 700 );
	} while( !(newBits & (1U<<WIFI_SEND_OK)) && ++num<4 );
	//----�ȴ�DMA���Ϳ��� end -----
}

static int send_at_command_a_single_reply( char *cmd, char *target ) {
	uint8_t times;
	uint16_t num;
		
	sprintf( (char*)ws.txBuff, "%s\r\n", cmd );
	
	for ( times=5; times>0; times-- ) {
		ws.askConfig = 0;
		HAL_UART_AbortReceive( &huart2 );
		memset( ws.rxBuff, 0, WIFI_BUFF_SIZE );
		vTaskDelay(10);
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		for ( num=100; !ws.askConfig&&num>0; num-- ) { vTaskDelay(5); }
		if ( !ws.askConfig ) continue;
		for ( num=100; strstr((char*)ws.rxBuff,target)==NULL&&num>0; num-- ) { vTaskDelay(5); }
		if ( num > 0 )
			break;
	}
	if ( times > 0 )
		return 0;
	return -1;
}
static int send_command_check_res( char *cmd, char *resTrue ) {
	uint8_t times = 5;
	uint16_t num;
	
	sprintf( (char*)ws.txBuff, "%s\r\n", cmd );
	for ( times=2; times>0; times-- ) {
		HAL_UART_AbortReceive( &huart2 );
		ws.askConfig = 0;
		memset( ws.rxBuff, 0, WIFI_BUFF_SIZE );
		vTaskDelay(10);
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		for ( num=100; !ws.askConfig&&num>0; num-- ) { vTaskDelay(10); }
		if ( !ws.askConfig ) continue;
		for ( num=100; strstr((char*)ws.rxBuff,resTrue)==NULL&&num>0; num-- ) { vTaskDelay(10); }
		if ( num > 0 ) break;
		if ( !num && strstr((char*)ws.rxBuff,"OK")!=NULL ) {
			times = 0;
			break;
		}
	}
	if ( times > 0 )
		return 0;
	return -1;
}
static void wifi_smartConfig( void ) {
	char *ppos;
	uint8_t i;
	uint16_t cont;
	
	taskENTER_CRITICAL();
	memset(wifi_str.rxBuff, 0, DEBUG_BUFF_SIZE);
	vTaskDelay(100);
	printf( "\r\n------\r\n������������\r\n------\r\n" );
	taskEXIT_CRITICAL();
	//����start(����˸)
	send_at_command_a_single_reply("AT+CWMODE=1", "OK" );
	//��������:  AT+CWSTARTSMART=3
	
	for( ;;	) {
		cont = 250;//25s
		ws.askConfig = 0;
		sprintf((char*)ws.txBuff, "AT+CWSTOPSMART\r\n");
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;}
		vTaskDelay(100);HAL_UART_AbortReceive( &huart2 );
		ws.askConfig = 0;
		sprintf((char*)ws.txBuff, "AT+CWSTARTSMART=3\r\n");
		HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
		HAL_UART_Transmit( &huart2, ws.txBuff, strlen((char*)ws.txBuff), portMAX_DELAY );
		while( !ws.askConfig ){;;} 
		vTaskDelay(50);
		//ws.len = WIFI_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		if ( !buffCompareToBuff( "\r\nOK\r\n", (char*)ws.rxBuff, 6 ) ) {
			vTaskDelay(100);
			HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
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
				write_to_flash();//�洢�˺�����
				break;
			}
		}
		printf("\r\n----- \r\n%s\r\n", ws.rxBuff);
		if ( cont == 0 ) {
			xTaskNotify( led_taskHandle, 1U<<LED_SLOW_FLASHING, eSetBits );//led����,����wifi
			vTaskDelay(10);
			continue;//��ʱδ����,�������,���¿���
		}
		break;
	}
	//�˳���������:   AT+CWSTOPSMART
	send_at_command_a_single_reply("AT+CWSTOPSMART", "OK" );

	//����end(�ƹر�)
}

static void reset_rx_dma( void ) {
	HAL_UART_AbortReceive( &huart2 );
	ws.askConfig = 0;
	memset( ws.rxBuff, 0, WIFI_BUFF_SIZE );
	vTaskDelay(10);
	HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
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
			printf( "wifi����ʧ��!\r\n" );
			return -1;
		} else if ( buffCompareToBuff( "OK\r\n", (char *)&ws.rxBuff[ws.len - strlen("OK\r\n")], strlen("OK\r\n") ) ) {
			printf( "wifi���ӳɹ�!\r\n" );
			return 0;
		} else {
			HAL_UART_Receive_DMA( &huart2, ws.rxBuff, WIFI_BUFF_SIZE );//����DMA����
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
				//��ȡ�����µ�ʱ��
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
	if ( ps.endHour > ps.startHour ) {//һ�쵱��
		if ( ps.nowHour>=ps.startHour && ps.nowHour<ps.endHour ) {
			ps.runTime = ( ps.endHour - (ps.nowHour+1) )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			return 0;
		} else {
			return -1; 
		}
	} else {//��ֹʱ���һ��
		if ( ( ps.nowHour>=ps.startHour && ps.nowHour<=23 ) || ( ps.nowHour<ps.endHour ) ) {
			if ( ps.nowHour>=ps.startHour && ps.nowHour<=23 ) {
				ps.runTime = ( 24 - (ps.nowHour+1) + ps.endHour )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			} else {
				ps.runTime = ( ps.endHour - (ps.nowHour+1) )*3600000 + (60-ps.nowMin)*60000 | UNTIL_TIME;
			}
			//����ػ�ʱ��,����֪ͨ
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
	for( i=0; i<2; i++ ) {
		if ( session[i].onlineSta && session[i].pid==pid ) {
			session[i].onlineSta = 0;
			session[i].dir = 0;
			return;
		}
	}
	xTaskNotify( check_online_taskHandle, 1U<<WIFI_CHECK_ONLINE, eSetBits );
}
static SESSION* find_sessoin_by_pid( uint8_t pid ) {
	uint8_t i;
	for( i=0; i<2; i++ ) {
		if ( session[i].onlineSta && session[i].pid==pid ) {
			session[i].heart = DEFAULT_HEART;
			return &session[i];
		}
	}
	for( i=0; i<2; i++ ) {
		if ( !session[i].onlineSta ) {
			session[i].onlineSta = 1;
			session[i].pid = pid;
			session[i].heart = DEFAULT_HEART;
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
			wifi_tcp_send( "�ѷ��Ϳ���ָ��\r\n", strlen("�ѷ��Ϳ���ָ��\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "2", sessp->data, 1 ) ) {
			xTaskNotify( pwm_taskHandle, 1U<<TURN_OFF, eSetBits );
			wifi_tcp_send( "�ѷ��͹ص�ָ��\r\n", strlen("�ѷ��͹ص�ָ��\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "3", sessp->data, 1 ) ) {
			xTaskNotify( pwm_taskHandle, 1U<<FLASHING, eSetBits );
			wifi_tcp_send( "�ѷ�����˸ָ��\r\n", strlen("�ѷ�����˸ָ��\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "4", sessp->data, 1 ) ) {
			wifi_tcp_send( "���ܴ�����...\r\n", strlen("���ܴ�����...\r\n"), sessp->pid, cmd_handle_taskHandle );
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
uint8_t cmd_sub_1_fun( void ) {//��������
	SESSION *sessp;
	uint8_t num;
	//��.��������ҳ:\r\n1.�������ʱ��\r\n2.��������ʱ��\r\n3.�����������\r\n��#�ŷ�����һ��˵�
	if ( xQueueReceive( cmd_queueHandle, &sessp, 10 ) ) {
		if ( buffCompareToBuff( "1", sessp->data, 1 ) ) {
			wifi_tcp_send( "���ܴ�����...\r\n", strlen("���ܴ�����...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "2", sessp->data, 1 ) ) {
			wifi_tcp_send( "���ܴ�����...\r\n", strlen("���ܴ�����...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		else if ( buffCompareToBuff( "3", sessp->data, 1 ) ) {
			wifi_tcp_send( PAGE_MENU_3, strlen(PAGE_MENU_3), sessp->pid, cmd_handle_taskHandle );
			ws.sesp->dir = 2;
		}
		else if ( buffCompareToBuff( "#", sessp->data, 1 ) || buffCompareToBuff( "��", sessp->data, 1 ) ) {
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
	//��.�����������ҳ:\r\n���������ֵ���������Ȱٷֱ� 20 - 100 ֮��\r\n����#�ż�������һ��˵�
	if ( xQueueReceive( cmd_queueHandle, &sessp, 10 ) ) {
		sessp->data[ sessp->dataLen ] = 0;
		t = ( uint8_t )str_to_u32( sessp->data );
		if ( t >= 20 && t <= 100 ) {
			pwm_str.maxBtnPer = t;
			str = pvPortMalloc( 100 );
			sprintf( str, "�ɹ������������Ϊ: %d%%", pwm_str.maxBtnPer );
			wifi_tcp_send( str, strlen(str), sessp->pid, cmd_handle_taskHandle );
			vPortFree( str );
			wifi_tcp_send( "��.�����������ҳ:\r\n���������ֵ���������Ȱٷֱ� 20 - 100 ֮��\r\n����#�ż�������һ��˵�", strlen("��.�����������ҳ:\r\n���������ֵ���������Ȱٷֱ� 20 - 100 ֮��\r\n����#�ż�������һ��˵�"), sessp->pid, cmd_handle_taskHandle );
		} 
		else if ( buffCompareToBuff( "#", sessp->data, 1 ) || buffCompareToBuff( "��", sessp->data, 1 ) ) {
			wifi_tcp_send( "��.��������ҳ:\r\n1.�������ʱ��\r\n2.��������ʱ��\r\n3.�����������\r\n��#�ŷ�����һ��˵�", strlen("��.��������ҳ:\r\n1.�������ʱ��\r\n2.��������ʱ��\r\n3.�����������\r\n��#�ŷ�����һ��˵�"), sessp->pid, cmd_handle_taskHandle );
			ws.sesp->dir = 1;
		}
		else {
			wifi_tcp_send( "��.�����������ҳ:\r\n���������ֵ���������Ȱٷֱ� 20 - 100 ֮��\r\n����#�ż�������һ��˵�", strlen("��.�����������ҳ:\r\n���������ֵ���������Ȱٷֱ� 20 - 100 ֮��\r\n����#�ż�������һ��˵�"), sessp->pid, cmd_handle_taskHandle );
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
			wifi_tcp_send( "���ܴ�����...\r\n", strlen("���ܴ�����...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		if ( buffCompareToBuff( "2", sessp->data, 1 ) ) {
			wifi_tcp_send( "���ܴ�����...\r\n", strlen("���ܴ�����...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
		if ( buffCompareToBuff( "3", sessp->data, 1 ) ) {
			wifi_tcp_send( "���ܴ�����...\r\n", strlen("���ܴ�����...\r\n"), sessp->pid, cmd_handle_taskHandle );
		}
	}
	num = uxQueueMessagesWaiting( cmd_queueHandle );
	return num;
}







