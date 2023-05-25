#ifndef __BSP_WIFI__H
#define __BSP_WIFI__H

#include "../Bsp/common.h"

typedef struct _WIFI_STR {
	uint8_t isConfig;
	uint8_t askConfig;
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint32_t len;
} WIFI_STR;


extern WIFI_STR wifi_str;


#define WIFI_BUFF_SIZE	1024

/*---- wifi task bits start ----------------*/
#define WIFI_DEVICE_INIT 	0
#define WIFI_PARSE_DATA 	1
#define WIFI_SEND_OK			2
/*---- wifi task bits end ----------------*/


void wifi_init( void );
void wifi_parse_data( void );




#endif /*__BSP_WIFI__H*/






