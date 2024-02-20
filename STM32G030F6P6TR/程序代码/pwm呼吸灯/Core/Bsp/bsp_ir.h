#ifndef __BSP_IR__
#define __BSP_IR__

#include "../common.h"

#define UP					0x01FE
#define DOWN			0x02FD
#define LEFT				0x06F9
#define RIGHT			0x07F8
#define OK					0x05FA
#define BLEFT			0x03FC
#define BRIGHT		0x04FB

#define LIGHT_ENHANCE_1		UP
#define LIGHT_WEAKEN_1		DOWN
#define LIGHT_ENHANCE_2		RIGHT
#define LIGHT_WEAKEN_2		LEFT
#define LIGHT_TOOGLE_1		OK
#define LIGHT_TOOGLE_2		BRIGHT
#define LED_MODE						BLEFT	

#define IR_DATA_PARSE			0

typedef struct _IR_STR {
	uint8_t rxDr;//1:一帧数据接收完毕 0:不解析
	uint8_t startFlag;//1:解码开始  0:非解码
	uint8_t count;//260us计数次数
	uint8_t irRepet;//0非重复码 1重复码
	uint16_t checkRepet;//重复码计数
	uint8_t irData[4];//用户码2个字节+键值码2个字节
	uint8_t irBitData[32];//4字节32位计时数组
} IR_STR;

extern IR_STR ir_str;


int8_t ir_data_parse( void );
void execute_led( void );


#endif /*__BSP_IR__*/


