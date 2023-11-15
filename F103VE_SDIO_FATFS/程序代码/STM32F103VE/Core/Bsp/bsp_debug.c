#include "../Bsp/bsp_debug.h"
#define ds debug_str
DEBUG_STR debug_str;
BYTE read_buf_debug[1024] = {0};

void debug_init( void ) {
	//uint32_t sta;
	taskENTER_CRITICAL();
	//ds.rxBuff = pvPortMalloc( 24 );
	ds.rxBuff0 = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.rxBuff1 = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	//开启DMA接收 开启串口空闲中断
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_IDLE );//使能空闲中断
	//__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_TC );//使能发送完毕中断
	//__HAL_DMA_ENABLE_IT( &DEBUG_HDMA_USART_RX, DMA_IT_HT | DMA_IT_TC );//DMA半传输中断/传输中断
	//__HAL_DMA_ENABLE_IT( &DEBUG_HDMA_USART_RX, DMA_IT_TC );//DMA传输中断
	//sta = HAL_DMA_RegisterCallback( &DEBUG_HDMA_USART_RX, HAL_DMA_XFER_HALFCPLT_CB_ID, dma_halfcplt_fun );
	//sta = HAL_DMA_RegisterCallback( &DEBUG_HDMA_USART_RX, HAL_DMA_XFER_CPLT_CB_ID, dma_cplt_fun );	
	ds.p_buf = ds.buf;
	ds.useBuff = 0;
	HAL_UART_AbortReceive( &DEBUG_HUART );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, DEBUG_BUFF_SIZE );//启动DMA接收
}

void dma_tc_fun( void ) {
	uint32_t counter;
//	char str[13] = {0};
//	memcpy(str, &ds.rxBuff[12], 12);
//	printf("全传输: %s\r\n", str);
	//printf("%s", )
	//char str[13] = {0};
	if ( ds.useBuff == 0 ) {
		ds.useBuff = 1;
		//HAL_UART_AbortReceive( &DEBUG_HUART );
		__HAL_TIM_SET_AUTORELOAD( &htim4, 65534 );//设置定时器自动重装载的值, 到该值后重新计数
		__HAL_TIM_SET_COUNTER( &htim4, 0 );//设置定时器初始值
		HAL_TIM_Base_Start( &htim4 );//启动定时器
		
		HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff1, 1024 );
		//memcpy(ds.p_buf, ds.rxBuff0, 12);
		//ds.p_buf += 12;
		//memset(ds.rxBuff0, 0, 12 );
	} else if ( ds.useBuff == 1 ) {
		ds.useBuff = 0;
		//HAL_UART_AbortReceive( &DEBUG_HUART );
		counter = __HAL_TIM_GET_COUNTER(&htim4);//获取定时器当前计数值
		HAL_TIM_Base_Stop( &htim4 );//停止定时器
		printf("接收1024字节总用时: %d us\r\n", counter);
		
		
		HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, 1024 );
		//memcpy(ds.p_buf, ds.rxBuff1, 12);
		//ds.p_buf += 12;
		//memset(ds.rxBuff1, 0, 12 );
		
		
	}
}

void test_uart_it_tc( void ) {
	//char str[13] = {0};
	HAL_UART_AbortReceive( &DEBUG_HUART );
	vTaskDelay(10);
	ds.len0 = 12 - __HAL_DMA_GET_COUNTER( &DEBUG_HDMA_USART_RX );
	
//	if ( ds.len0 < 12 ) {
//		memcpy(str, ds.rxBuff, ds.len0);
//		printf("\r\n剩余: %s\r\n", str);
//	} else if ( ds.len0 > 12 ) {
//		memcpy(str, &ds.rxBuff[12], ds.len0-12);
//		printf("\r\n剩余: %s\r\n", str);
//	} else {
//		printf("end\r\n");
//	}
	if ( ds.len0 != 12 ) {
		if ( ds.useBuff == 0 ) {
			memcpy(ds.p_buf, ds.rxBuff0, 12);
		} else if ( ds.useBuff == 1 ) {
			memcpy(ds.p_buf, ds.rxBuff1, 12);
		}
	} 
	printf("%s", ds.buf);
	memset( ds.buf, 0, 1024 );
	memset( ds.rxBuff0, 0, 12 );
	memset( ds.rxBuff1, 0, 12 );
	ds.useBuff = 0;
	ds.p_buf = ds.buf;
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, 12 );//启动DMA接收
}

void debug_parse_data_fun( void ) {
	#define USERPath "0:/"
	FRESULT ret;
	char *p_str;
	uint32_t num;
	
	HAL_UART_AbortReceive( &DEBUG_HUART );
	ds.len0 = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &DEBUG_HDMA_USART_RX );
	
	if ( u_str.sdInit && !u_str.transferStar ) {//已就绪,未开始传输
		p_str =  strstr( (char *)ds.rxBuff0, "size:" );
		if ( p_str != NULL ) {
			p_str += 5;
			u_str.size = 0;
			while ( *p_str>='0' && *p_str<='9' ) {
				u_str.size *= 10;
				u_str.size += *p_str-'0';
				p_str++;
			}
			printf("size:%d\r\n", u_str.size );
			p_str++;
			if ( strstr( (char *)p_str, "name:" ) != NULL ) {
				p_str += 5;
				sprintf( u_str.name, "%s", p_str );
				printf("name:%s\r\n", u_str.name );
			}
			
		} else if ( strstr( (char *)ds.rxBuff0, "name:" ) != NULL ) {
			p_str += 5;
			sprintf( u_str.name, "%s", p_str );
			printf("name:%s\r\n", u_str.name );
		}
		if ( u_str.size!=0 && strlen((char*)u_str.name)!=0 ) {
			ret = f_open( &SDFile, u_str.name, FA_CREATE_ALWAYS | FA_WRITE );
			if ( ret == FR_OK ) {
				u_str.transferStar = 1;
				printf( "transfer start...\r\n" );
			} else {
				printf( "open file err\r\n" );
			}
		}
	} else if ( u_str.sdInit && u_str.transferStar ) {//已就绪,正在传输
		vTaskDelay(100);
		printf("接收到的数据 %s\r\n", ds.rxBuff0);
		ret = f_write(&SDFile, ds.rxBuff0, strlen((char*)ds.rxBuff0), &num);
		f_close(&SDFile);
		
		ret = f_open(&SDFile, u_str.name, FA_OPEN_EXISTING | FA_READ);
		ret = f_read(&SDFile, read_buf_debug, sizeof(read_buf_debug), &num);
		if ( ret == FR_OK ) 
			printf("read data:\"%s\"!\r\nread len：%d\r\n", read_buf_debug, num);
		f_close(&SDFile);
		
		
	} else {//未就绪
		
		if ( strstr( (char *)ds.rxBuff0, "f_mount" ) != NULL ) {
			ret = f_mkfs(USERPath, 0, 0);
			if( ret == FR_OK ) {
				printf("格式化成功！\r\n");
				/* 先取消挂载，后重新挂载 */
				f_mount(NULL, USERPath, 1); 
				ret = f_mount(&SDFatFS, USERPath, 1);
				u_str.sdInit = 1;
			} else {
				printf("格式化失败！\r\n");
				f_mount(NULL, USERPath, 1);
			}
			
			
		}
		
//		if ( strstr( (char *)ds.rxBuff, "test" ) != NULL ) {
//			my_fatfs_test_fun();
//		}
		
	}
	
	memset( ds.rxBuff0, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, DEBUG_BUFF_SIZE );//启动DMA接收
}


