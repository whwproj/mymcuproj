#include "../Bsp/bsp_debug.h"
#define ds debug_str
DEBUG_STR debug_str;

void debug_init( void ) {
	//uint32_t sta;
	taskENTER_CRITICAL();
	//ds.rxBuff = pvPortMalloc( 24 );
	ds.rxBuff0 = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.rxBuff1 = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	//开启DMA接收 开启串口空闲中断
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_IDLE );//使能空闲中断
	__HAL_UART_ENABLE_IT( &DEBUG_HUART, UART_IT_TC );//使能发送完毕中断
	ds.useBuff = 0;
	HAL_UART_AbortReceive( &DEBUG_HUART );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, DEBUG_BUFF_SIZE );//启动DMA接收
}

void dma_tc_fun( void ) {
	uint32_t num;
	//FRESULT ret;
	
	if ( ds.useBuff == 0 ) {
		ds.useBuff = 1;
		HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff1, DEBUG_BUFF_SIZE );
		//写入文件系统
		f_write(&SDFile, ds.rxBuff0, DEBUG_BUFF_SIZE, &num);
		u_str.wSize += num;
		memset( ds.rxBuff0, 0,  DEBUG_BUFF_SIZE );
		
	} else if ( ds.useBuff == 1 ) {
		ds.useBuff = 0;
		HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, DEBUG_BUFF_SIZE );
		//写入文件系统
		f_write(&SDFile, ds.rxBuff1, DEBUG_BUFF_SIZE, &num);
		u_str.wSize += num;
		memset( ds.rxBuff1, 0,  DEBUG_BUFF_SIZE );
	}
}

//void test_uart_it_tc( void ) {
//	//char str[13] = {0};
//	HAL_UART_AbortReceive( &DEBUG_HUART );
//	vTaskDelay(10);
//	ds.len0 = 12 - __HAL_DMA_GET_COUNTER( &DEBUG_HDMA_USART_RX );
//	
////	if ( ds.len0 < 12 ) {
////		memcpy(str, ds.rxBuff, ds.len0);
////		printf("\r\n剩余: %s\r\n", str);
////	} else if ( ds.len0 > 12 ) {
////		memcpy(str, &ds.rxBuff[12], ds.len0-12);
////		printf("\r\n剩余: %s\r\n", str);
////	} else {
////		printf("end\r\n");
////	}
//	if ( ds.len0 != 12 ) {
//		if ( ds.useBuff == 0 ) {
//			memcpy(ds.p_buf, ds.rxBuff0, 12);
//		} else if ( ds.useBuff == 1 ) {
//			memcpy(ds.p_buf, ds.rxBuff1, 12);
//		}
//	} 
//	printf("%s", ds.buf);
//	memset( ds.buf, 0, 1024 );
//	memset( ds.rxBuff0, 0, 12 );
//	memset( ds.rxBuff1, 0, 12 );
//	ds.useBuff = 0;
//	ds.p_buf = ds.buf;
//	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, 12 );//启动DMA接收
//}

void debug_parse_data_fun( void ) {
	#define USERPath "0:/"
	FRESULT ret;
	char *p_str;
	uint32_t num;
	
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &DEBUG_HDMA_USART_RX );
	
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
				//开始接收数据
				memset( ds.rxBuff0, 0, DEBUG_BUFF_SIZE );
				HAL_UART_AbortReceive( &DEBUG_HUART );
				HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, DEBUG_BUFF_SIZE );//启动DMA接收
				return;
			} else {
				printf( "open file err\r\n" );
			}
		}
	} else if ( u_str.sdInit && u_str.transferStar ) {//已就绪,正在传输
		ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER( &DEBUG_HDMA_USART_RX );
		if ( ds.len != DEBUG_BUFF_SIZE ) {
			if ( ds.useBuff == 0 ) {
				u_str.wSize += ds.len;
				if ( u_str.wSize < u_str.size ) {
					ds.useBuff = 1;
					HAL_UART_AbortReceive( &DEBUG_HUART );
					HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff1, DEBUG_BUFF_SIZE );
				}
				//写入文件系统
				ret = f_write(&SDFile, ds.rxBuff0, ds.len, &num);
				memset( ds.rxBuff0, 0,  DEBUG_BUFF_SIZE );
			} else if ( ds.useBuff == 1 ) {
				u_str.wSize += ds.len;
				if ( u_str.wSize < u_str.size ) {
					ds.useBuff = 0;
					HAL_UART_AbortReceive( &DEBUG_HUART );
					HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, DEBUG_BUFF_SIZE );
				}
				//写入文件系统
				ret = f_write(&SDFile, ds.rxBuff1, ds.len, &num);
				memset( ds.rxBuff1, 0,  DEBUG_BUFF_SIZE );
			}
		}
		//写入完成
		if ( u_str.wSize >= u_str.size ) {
			f_close(&SDFile);
			f_mount(NULL, USERPath, 1);
			HAL_UART_AbortReceive( &DEBUG_HUART );
			memset( ds.rxBuff0, 0,  DEBUG_BUFF_SIZE );
			HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, DEBUG_BUFF_SIZE );
			printf("finished total size: %dByte\r\nenter f_mount to again\r\n", u_str.wSize);
			u_str.transferStar = 0;
			u_str.sdInit = 0;
		} 
		return;
		
//		ret = f_open(&SDFile, u_str.name, FA_OPEN_EXISTING | FA_READ);
//		if ( ret == FR_OK ) {
//			ret = f_read(&SDFile, ds.rxBuff, 1024, &num);
//			printf("%s", ds.rxBuff);
//		}
//		ret = f_read(&SDFile, ds.rxBuff, 3840, &num);
//		printf("%s", ds.rxBuff);
//		f_mount(NULL, USERPath, 1);
		
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
	}
	
	memset( ds.rxBuff0, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &DEBUG_HUART, ds.rxBuff0, DEBUG_BUFF_SIZE );//启动DMA接收
}


