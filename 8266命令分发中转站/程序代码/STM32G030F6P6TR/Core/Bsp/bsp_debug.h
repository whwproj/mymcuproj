#ifndef __BSP_DEBUG__H
#define __BSP_DEBUG__H

#include "../common.h"

#define DEBUG_HUART					huart2
#define DEBUG_HDMA_USART_RX	hdma_usart2_rx
#define DEBUG_HDMA_USART_TX	hdma_usart2_tx
#define DEBUG_BUFF_SIZE	256

/*---- debug task bits start ----------------*/
#define DEBUG_DEVICE_INIT 0
#define DEBUG_PARSE_DATA 	1
#define DEBUG_SEND_OK 		2
/*---- debug task bits end ----------------*/

typedef struct _DEBUG_STR {
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint32_t len;
} DEBUG_STR;

extern DEBUG_STR debug_str;


void debug_init( void );
void debug_parse_data_fun( void );

#endif /*__BSP_DEBUG__H*/






