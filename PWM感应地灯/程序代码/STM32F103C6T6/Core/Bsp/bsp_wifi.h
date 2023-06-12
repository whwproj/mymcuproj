#ifndef __BSP_WIFI__H
#define __BSP_WIFI__H

#include "../Bsp/common.h"

#define PAGE_MENU_1 "一.主页:\r\n请输入数字执行相应命令:\r\n1.开灯\r\n"\
										"2.关灯\r\n3.闪烁\r\n4.自动检测\r\n5.参数设置"
										
#define PAGE_MENU_2 "二.参数设置页:\r\n1.设置天黑时间\r\n"\
										"2.设置天亮时间\r\n3.调节最高亮度\r\n按#号返回主菜单"
										
#define PAGE_MENU_3 "三.调节最高亮度页:\r\n请输入数字调节最高亮度百分比"\
										"20 - 100 之间\r\n按下#号键返回上一层菜单"								

typedef struct _SESSION {
	uint8_t onlineSta;//0:offline 1:online
	uint8_t pid;
	uint8_t dir;//main:0 sub1:1...
	uint8_t dataLen;
	char data[128];
} SESSION;

typedef struct _WIFI_STR {
	uint8_t isConfig;
	uint8_t askConfig;
	uint8_t passDataStop;//1:暂停数据解析
	//uint8_t communication;//0:wifi未在通信即wifi空闲,可查询状态 1:不可查询,避免数据混乱
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint32_t len;
	uint16_t dLen;
	SESSION *sesp;
} WIFI_STR;



extern WIFI_STR wifi_str;
extern SESSION session[];

#define WIFI_BUFF_SIZE	256
#define WIFI_TXBUFF_SIZE	384

/*---- wifi task bits start ----------------*/
#define WIFI_DEVICE_INIT 	0
#define WIFI_PARSE_DATA 	1
#define WIFI_SEND_OK			31
/*---- wifi task bits end ----------------*/

/*---- sleep task bits start ----------------*/
#define FALL_SLEEP 	0
#define WAKE_UP			1
/*---- sleep task bits end ----------------*/

/*---- cmd task bits start ----------------*/
#define CMD_MAIN		0
#define CMD_SUB_1		1
#define CMD_SUB_2		2
#define CMD_SUB_3		3
/*---- cmd task bits end ----------------*/

/*---- check task bits start ----------------*/
#define TO_CHECK_ONLINE				0
#define WIFI_IS_COMMUNICATION 1
/*---- check task bits end ----------------*/

uint32_t wifi_check_online( void );
void wifi_to_reconfigure( void );
void wifi_init( void );
void wifi_parse_data( void );
int check_turn_on_time( void );
uint8_t cmd_main_fun( void );
uint8_t cmd_sub_1_fun( void );
uint8_t cmd_sub_2_fun( void );
uint8_t cmd_sub_3_fun( void );
#endif /*__BSP_WIFI__H*/






