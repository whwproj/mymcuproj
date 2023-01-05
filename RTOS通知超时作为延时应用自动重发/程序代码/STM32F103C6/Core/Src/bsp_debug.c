#include "bsp_debug.h"

extern QueueHandle_t Debug_RxData_QueueHandle;//Debug回传数据消息队列
extern QueueHandle_t WH_LTE_Resend_QueueHandle;//4G模块自动重发消息队列
extern TaskHandle_t WH_LTE_ResendTaskHandle;//4G模块自动重发消息任务

uint8_t *Debug_DMA_rBuff;//DMA接收缓存指针

DEBUG_DATA debug_data;

/*------------------------- Debug串口接收中断回调处理函数 start -------------------------*/
void Debug_ISR_RxCallback( void ) {
	uint16_t dataLen;
	BaseType_t pxHigherPriorityTaskWoken;
	dataLen = Debug_rBuff_Size - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	memcpy((void *)debug_data.data, (void *)Debug_DMA_rBuff, dataLen);
	debug_data.size = dataLen;
	
	xQueueSendFromISR(Debug_RxData_QueueHandle, (void *)&debug_data, &pxHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);

	HAL_UART_DMAStop(&huart1); 
	HAL_UART_Receive_DMA(&huart1, Debug_DMA_rBuff, Debug_rBuff_Size);//再次开启DMA接收
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);  //再次使能串口空闲中断


#if 0//4G模块透传调试
	dataLen = Debug_rBuff_Size - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	if ( Debug_DMA_rBuff[dataLen-1] == '@' ) {
		Debug_DMA_rBuff[dataLen-1] = '\0';

		xQueueSendFromISR(Debug_RxData_QueueHandle, Debug_DMA_rBuff, &pxHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);

		HAL_UART_DMAStop(&huart1); 
		HAL_UART_Receive_DMA(&huart1, Debug_DMA_rBuff, Debug_rBuff_Size);//再次开启DMA接收
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);  //再次使能串口空闲中断
	}
#endif
}
/*------------------------- Debug串口接收中断回调处理函数 end -------------------------*/

/*------------------------- Debug调试串口解析处理函数 start -------------------------*/
static BaseType_t buffCompareToBuff(char *buff1, char *buff2){
	uint16_t size = strlen(buff1);
	if ( size == 0 ) return pdFALSE;
	while(size--) {
		if ( *buff1 != *buff2 ) return pdFALSE;
		buff1++;
		buff2++;
	}
	return pdTRUE;
}
BaseType_t Debug_ParseRxData( BaseType_t *pRx ) {

	DEBUG_DATA *pTypeData = (DEBUG_DATA *)pRx;
	char *pdata = (char*)pTypeData->data;
	uint8_t size = pTypeData->size;
	
	if ( size==2 && buffCompareToBuff((char *)&"ok", pdata) ) {
		//此处模拟发送回复ok的事件信号
		xTaskNotify(WH_LTE_ResendTaskHandle, 1, eSetValueWithOverwrite);
		
		
	} else if ( size==strlen("丢弃") && buffCompareToBuff((char *)&"丢弃", pdata) ) {
		//此处模拟放弃数据发送新数据
		xTaskNotify(WH_LTE_ResendTaskHandle, 2, eSetValueWithOverwrite);

	} else {
		xQueueSend( WH_LTE_Resend_QueueHandle, (void *)pRx, 0xFFF );
	}
	
	
	//HAL_UART_Transmit(&huart1, (uint8_t *) pdata, pTypeData->size, 0xffff);
	
//	printf_port = 1;printf("Debug串口收到的数据：%s\r\n", pdata);
//	HAL_UART_Transmit(&huart1, (uint8_t *) "Debug串口收到的数据：", sizeof("Debug串口收到的数据："), 0xffff);
//	HAL_UART_Transmit(&huart1, (uint8_t *) pdata, pTypeData->size, 0xffff);
//	HAL_UART_Transmit(&huart1, (uint8_t *) "\r\n", sizeof("\r\n"), 0xffff);
	
	return 0;
}
/*------------------------- Debug调试串口解析处理函数 end -------------------------*/

/*--*/

