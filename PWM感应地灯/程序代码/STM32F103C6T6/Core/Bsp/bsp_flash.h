#ifndef __BSP__FLASH__H
#define __BSP__FLASH__H

#include "../Bsp/common.h"

typedef struct _PARAM_STR {
	char wifiName[50];
	char wifiPasswd[30];
} PARAM_STR;

extern PARAM_STR param_str;

#endif /*__BSP__FLASH__H*/



/**/


