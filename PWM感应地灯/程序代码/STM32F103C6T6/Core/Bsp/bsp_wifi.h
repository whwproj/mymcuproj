#ifndef __BSP_WIFI__H
#define __BSP_WIFI__H

#include "../Bsp/common.h"

typedef struct _WIFI_STR {
	uint8_t isConfig;
	uint8_t askConfig;
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint32_t len;
	
	uint8_t nowHour;
	uint8_t nowMin;
	uint8_t startHour;
	uint8_t startMin;
	uint8_t endHour;
	uint8_t endMin;
	uint32_t runTime;//0x10000000
} WIFI_STR;


extern WIFI_STR wifi_str;

#define UNTIL_TIME 0x10000000
#define WIFI_BUFF_SIZE	128

/*---- wifi task bits start ----------------*/
#define WIFI_DEVICE_INIT 	0
#define WIFI_PARSE_DATA 	1
#define WIFI_SEND_OK			2
/*---- wifi task bits end ----------------*/


void wifi_init( void );
void wifi_parse_data( void );
int check_turn_on_time( void );

#endif /*__BSP_WIFI__H*/






