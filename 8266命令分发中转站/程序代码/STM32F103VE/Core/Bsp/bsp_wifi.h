#ifndef __BSP_WIFI__H
#define __BSP_WIFI__H

#include "../common.h"

//#define WIFI_NAME0		"UFI-Dev"
//#define WIFI_PASSWD0	"Ufidev888"
#define WIFI_NAME0		"Tenda_ECE950_5G"
#define WIFI_PASSWD0	"12345678"
#define WIFI_NAME1		"MI"
#define WIFI_PASSWD1	"wuhanwei"

#define WIFIHUART huart3
#define WIFI_HDMA_HUART_RX 	hdma_usart3_rx
#define WIFI_HDMA_HUART_TX 	hdma_usart3_tx

#define mqtt_User_Name "thingidp@anvntlw|esp8266|0|MD5"
#define mqtt_passwd		"fcbfa8ffddfca54945ca549955cfa2cc"

#define TCP_URL_0	"anvntlw.iot.gz.baidubce.com"
#define TCP_PORT_0	1883
#define TCP_URL_1	"server.natappfree.cc"
#define TCP_PORT_1	36017

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
	uint16_t dLen;
	uint16_t tLen;
	uint8_t checkOnlineNum;
	SESSION *sesp;
	uint8_t tcp0_errnum;//tcp0重连次数,3次则重启设备
	uint8_t tcp1_errnum;//tcp1重连次数
	uint8_t heartBeatTime;//mqtt������
	
} WIFI_STR;

typedef struct _TCP_DATA {
	uint8_t data[256];
	uint16_t len;
} TCP_DATA;

extern WIFI_STR wifi_str;
extern SESSION session[];

#define WIFI_RXBUFF_SIZE	256
#define WIFI_TXBUFF_SIZE	256

#define DEFAULT_HEART	60

/*---- wifi task bits start ----------------*/
//*****	wifi_control_task_fun
#define WIFI_DEVICE_INIT 		0
#define WIFI_PARSE_DATA 		1
#define WIFI_CONNECT_TCP0_	2
#define WIFI_CONNECT_TCP1_	3
#define WIFI_TCP0_SEND			4
#define WIFI_TCP1_SEND			5
#define WIFI_SEND_HEART			6
#define WIFI_SEND_OK				7

//*****	wifi_control_task_fun 
#define WIFI_CONNECT_TCP0_DELAY	0
#define WIFI_CONNECT_TCP1_DELAY	1

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


extern WIFI_STR w_str;

uint32_t wifi_check_online( void );
void wifi_to_reconfigure( void );
void wifi_parse_data( void );
int check_turn_on_time( void );
uint8_t cmd_main_fun( void );
uint8_t cmd_sub_1_fun( void );
uint8_t cmd_sub_2_fun( void );
uint8_t cmd_sub_3_fun( void );


void esp_connect_tcp0 ( void );//连接TCP0
void esp_connect_tcp1 ( void );//连接TCP1
void wifi_tcp0_send_data( void );
void wifi_tcp1_send_data( void );
void wifi_mqtt_heart( void );
void mqtt_connect( void );//建立mqtt连接
void wifi_init( void );//WIFI��ʼ��
UBaseType_t wifi_mqtt_data_parse( void );//����mqtt����
#endif /*__BSP_WIFI__H*/






