#ifndef __BSP_JSON__H
#define __BSP_JSON__H

#include "../common.h"

#define PRINT_STR_0	"{\"pvcode\":"	//1
#define PRINT_STR_1	",\"errcode\":"		//true/false
#define PRINT_STR_2	",\"msg\":\""	//abc
#define PRINT_STR_END	"\"}"


char* cjson_reply_template( uint32_t pvcode, int errCode, char* msg );
void cjson_pase_method( uint8_t *pdBuff );//解析JSON数据



#endif /*__BSP_JSON__H*/

