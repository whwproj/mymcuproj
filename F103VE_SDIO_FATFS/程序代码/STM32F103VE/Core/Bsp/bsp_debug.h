#ifndef __BSP_DEBUG__H
#define __BSP_DEBUG__H

#include "../common.h"

#define DEBUG_HUART							huart1
#define DEBUG_HDMA_USART_RX		hdma_usart1_rx
//#define DEBUG_HDMA_USART_TX	hdma_usart1_tx
#define DEBUG_BUFF_SIZE	10240

/*---- debug task bits start ----------------*/
#define DEBUG_DEVICE_INIT 		0
#define DEBUG_PARSE_DATA 	1
#define DEBUG_SEND_OK 			2
#define D_DMA_HT_FUN				3
#define D_DMA_TC_FUN				4
#define D_TEST_UART_IT_TC		5

/*---- debug task bits end ----------------*/

typedef struct _DEBUG_STR {
	//uint8_t ht_tc_flag;//0/1 接收 0前半 1后半
	uint8_t useBuff;//0/1
	uint8_t *p_buf;
	uint8_t *rxBuff0;
	uint8_t *rxBuff1;
	uint32_t len;
} DEBUG_STR;

extern DEBUG_STR debug_str;

void debug_init( void );
void debug_parse_data_fun( void );
void debug_parse_data_fun2( void );

void dma_ht_fun( void );
void dma_tc_fun( void );
void test_uart_it_tc( void );
	  
#endif /*__BSP_DEBUG__H*/






