#ifndef __BSP_JSON__H
#define __BSP_JSON__H

#include "../common.h"

#define PRINT_STR_0	"{\"pvcode\":"	//1
#define PRINT_STR_1	",\"sta\":"		//true/false
#define PRINT_STR_2	",\"msg\":\""	//abc
#define PRINT_STR_END	"\"}"

char* cjson_pase_method( uint8_t *pdBuff );



#endif /*__BSP_JSON__H*/

