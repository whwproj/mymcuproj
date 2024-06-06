#ifndef __bsp_ir__
#define __bsp_ir__

#include "../common.h"

typedef struct _IR_STR {
	uint8_t rxDr;//1:一帧数据接收完毕 0:不解析
	uint8_t startFlag;//1:解码开始  0:非解码
	uint8_t count;//280us计数次数
	uint8_t irData[4];//用户码2个字节+键值码2个字节
	uint8_t irBitData[32];//4字节32位计时数组
} IR_STR;

extern IR_STR ir_str;


int8_t ri_data_parse( void );



#endif /*__bsp_ir__*/


