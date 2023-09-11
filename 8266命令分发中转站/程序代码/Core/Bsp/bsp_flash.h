#ifndef __BSP__FLASH__H
#define __BSP__FLASH__H

#include "../common.h"

typedef struct _PARAM_STR {
	char wifiName[50];
	char wifiPasswd[30];
	uint8_t nowHour;
	uint8_t nowMin;
	uint8_t startHour;
	uint8_t startMin;
	uint8_t endHour;
	uint8_t endMin;
	uint32_t runTime;//0x10000000
} PARAM_STR;

#define UNTIL_TIME 0x10000000

extern PARAM_STR param_str;

void write_to_flash( void );
void read_from_flash( void );

#endif /*__BSP__FLASH__H*/



/**/


