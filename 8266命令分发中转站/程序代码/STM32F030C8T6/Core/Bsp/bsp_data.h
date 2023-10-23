#ifndef __BSP_DATA__H
#define __BSP_DATA__H

#include "../common.h"

/*----------  ------------*/
//#define PARSE_WIFI_DATA			0
/*----------  ------------*/

/*-------------------- TIME start --------------------*/
#define NRF_SEND_TIME		0
/*-------------------- TIME end --------------------*/

typedef struct __DATA_STR {
	uint16_t code;
	uint8_t nrfid;
	char* data;
} DATA_STR;
extern DATA_STR data_str;

typedef struct __ASK_STR {
	uint8_t list;//每一位代表一个计时消息,低5位有效 xxx1 1111
	uint32_t sendTicks[5];//发送成功的时间戳
	uint8_t deviceIds[5];
	uint16_t pcode[5];
	uint8_t useing;
}	ASK_STR;
extern ASK_STR ask_str;

UBaseType_t parse_wifi_data_fun( void );//解析wifi数据
int add_data_to_list_fun( uint8_t deviceId, uint16_t pcode );//添加到计时队列
void nrf_send_time_fun( void );//发送数据成功后计时

#endif /*__BSP_DATA__H*/






