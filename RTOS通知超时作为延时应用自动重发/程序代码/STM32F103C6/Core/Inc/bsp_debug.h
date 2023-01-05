#ifndef __BSP_DEBUG__H
#define __BSP_DEBUG__H

#include "main.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "string.h"
#include <stdio.h>

#define Debug_rBuff_Size	300//接收缓存大小

typedef struct _DEBUG_DATA {
	uint8_t data[Debug_rBuff_Size];
	uint16_t size;
} DEBUG_DATA;

extern uint8_t *Debug_DMA_rBuff;//DMA接收缓存指针

void Debug_ISR_RxCallback( void );
BaseType_t Debug_ParseRxData( BaseType_t *pRx );//解析调试串口的数据

#endif /*__BSP_DEBUG__H*/




