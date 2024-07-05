#ifndef __BSP_WIFI__H
#define __BSP_WIFI__H

#include "../Bsp/common.h"

#define PAGE_MENU_1 "һ.��ҳ:\r\n����������ִ����Ӧ����:\r\n1.����\r\n"\
										"2.�ص�\r\n3.��˸\r\n4.�Զ����\r\n5.��������"
										
#define PAGE_MENU_2 "��.��������ҳ:\r\n1.�������ʱ��\r\n"\
										"2.��������ʱ��\r\n3.�����������\r\n��#�ŷ������˵�"
										
#define PAGE_MENU_3 "��.�����������ҳ:\r\n���������ֵ���������Ȱٷֱ�"\
										"20 - 100 ֮��\r\n����#�ż�������һ��˵�"								

typedef struct _SESSION {
	uint8_t onlineSta;//0:offline 1:online
	uint8_t pid;
	uint8_t dir;//main:0 sub1:1...
	uint8_t dataLen;
	uint8_t heart;//��ʼ10 �ݼ���0�ػ����򸲸�
	char data[128];
} SESSION;

typedef struct _WIFI_STR {
	uint8_t isConfig;
	uint8_t askConfig;
	uint8_t passDataStop;//1:��ͣ���ݽ���
	//uint8_t communication;//0:wifiδ��ͨ�ż�wifi����,�ɲ�ѯ״̬ 1:���ɲ�ѯ,�������ݻ���
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint32_t len;
	uint16_t dLen;
	uint8_t checkOnlineNum;
	SESSION *sesp;
} WIFI_STR;



extern WIFI_STR wifi_str;
extern SESSION session[];

#define WIFI_BUFF_SIZE	256
#define WIFI_TXBUFF_SIZE	384

#define DEFAULT_HEART	60

/*---- wifi task bits start ----------------*/
#define WIFI_DEVICE_INIT 	0
#define WIFI_PARSE_DATA 	1
#define WIFI_CHECK_ONLINE 2
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
#define NO_SESSION 						2
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






