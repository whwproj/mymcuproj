#ifndef __BSP_WIFI__H
#define __BSP_WIFI__H

#include "../Bsp/common.h"

typedef struct _SESSION {
	uint8_t onlineSta;//0:offline 1:online
	uint8_t pid;
	uint8_t dir;//main:0 sub1:1...
	char data[128];
} SESSION;

typedef struct _WIFI_STR {
	uint8_t isConfig;
	uint8_t askConfig;
	uint8_t passDataStop;//1:ÔÝÍ£Êý¾Ý½âÎö
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint32_t len;
	uint16_t dLen;
	SESSION *sesp;
} WIFI_STR;



extern WIFI_STR wifi_str;
extern SESSION session[];

#define WIFI_BUFF_SIZE	256

/*---- wifi task bits start ----------------*/
#define WIFI_DEVICE_INIT 	0
#define WIFI_PARSE_DATA 	1
#define WIFI_SEND_OK			2
/*---- wifi task bits end ----------------*/

/*---- sleep task bits start ----------------*/
#define FALL_SLEEP 	0
#define WAKE_UP			1
/*---- sleep task bits end ----------------*/

/*---- sleep task bits start ----------------*/
#define CMD_MAIN		0
#define CMD_SUB_1		1
#define CMD_SUB_2		2
#define CMD_SUB_3		3
/*---- sleep task bits end ----------------*/

void wifi_init( void );
void wifi_parse_data( void );
int check_turn_on_time( void );
uint8_t cmd_main_fun( void );
uint8_t cmd_sub_1_fun( void );
uint8_t cmd_sub_2_fun( void );
uint8_t cmd_sub_3_fun( void );
#endif /*__BSP_WIFI__H*/






