#ifndef __BSP_DEBUG__H
#define __BSP_DEBUG__H

#include "../common.h"

typedef struct _DEBUG_STR {
	uint8_t isConfig;
	uint8_t askConfig;
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint32_t len;
} DEBUG_STR;


extern DEBUG_STR debug_str;


#define DEBUG_BUFF_SIZE	128

/*---- debug task bits start ----------------*/
#define DEBUG_DEVICE_INIT 0
#define DEBUG_PARSE_DATA 	1
#define DEBUG_SEND_OK 		2

/*---- debug task bits end ----------------*/


void debug_init( void );
void debug_parse_data( void );
BaseType_t buffCompareToBuff(char *buff1, char *buff2, uint16_t len);



#endif /*__BSP_DEBUG__H*/






