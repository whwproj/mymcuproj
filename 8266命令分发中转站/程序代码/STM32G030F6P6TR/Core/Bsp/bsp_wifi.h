#ifndef __BSP_WIFI__H
#define __BSP_WIFI__H

#include "../common.h"

#define WIFI_NAME0		"UFI-Dev"
#define WIFI_PASSWD0	"Ufidev888"
//#define WIFI_NAME0		"Tenda_ECE950_5G"
//#define WIFI_PASSWD0	"12345678"
#define WIFI_NAME1		"MI"
#define WIFI_PASSWD1	"wuhanwei"

#define WIFIHUART huart1
#define WIFI_HDMA_HUART_RX 	hdma_usart1_rx
#define WIFI_HDMA_HUART_TX 	hdma_usart1_tx

#define mqtt_User_Name "thingidp@anvntlw|esp8266|0|MD5"
#define mqtt_passwd		"fcbfa8ffddfca54945ca549955cfa2cc"

#define TCP_URL_0	"anvntlw.iot.gz.baidubce.com"
#define TCP_PORT_0	1883
#define TCP_URL_1	"server.natappfree.cc"
#define TCP_PORT_1	36017

//#define HTML 	"HTTP/1.1 200 OK\r\n"\
//							"Content-Type: text/html\r\n"\
//							"Content-Length: 217\r\n"\
//							"Connection: keep-alive\r\n"\
//							"Accept-Ranges: bytes\r\n"\
//							"\r\n"\
//							"<!DOCTYPE html><html><head><meta charset=\"utf-8\">"\
//							"</head><body><form action=\"/\"><input type=\"text\" value=\"wifi名称\"><br />"\
//							"<input type=\"text\" value=\"wifi密码\"><br />"\
//							"<input type=\"submit\" value=\"提交\"></form></html>"
#define HTML "<!DOCTYPE html>"\
"<html>"\
	"<head>"\
		"<meta charset=\"utf-8\">"\
		"<meta name=\"viewport\"content=\"width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=0\">"\
	"</head>"\
	"<body><div style=\"display:block;margin:50px auto; width:180px;\">"\
		"<form action=\"/locahost\" method=\"POST\">"\
			"<input type=\"text\" id=\"wname\" placeholder=\"wifi名称\"><br />"\
			"<input type=\"text\" id=\"wpswd\" placeholder=\"wifi密码\"><br />"\
			"<input type=\"submit\" value=\"保存\"></form>"\
		"</div>"\
"</html>"
							
typedef struct _SESSION {
	uint8_t onlineSta;//0:offline 1:online
	uint8_t pid;
	uint8_t dir;//main:0 sub1:1...
	uint8_t dataLen;
	uint8_t heart;//锟斤拷始10 锟捷硷拷锟斤拷0锟截伙拷锟斤拷锟津覆革拷
	char data[128];
} SESSION;

typedef struct _WIFI_STR {
	uint8_t isConfig;
	uint8_t askConfig;
	uint8_t passDataStop;//1:锟斤拷停锟斤拷锟捷斤拷锟斤拷
	//uint8_t communication;//0:wifi未锟斤拷通锟脚硷拷wifi锟斤拷锟斤拷,锟缴诧拷询状态 1:锟斤拷锟缴诧拷询,锟斤拷锟斤拷锟斤拷锟捷伙拷锟斤拷
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint16_t dLen;
	uint16_t tLen;
	uint8_t checkOnlineNum;
	SESSION *sesp;
	uint8_t tcp0_errnum;//tcp0閲嶈繛娆℃暟,3娆″垯閲嶅惎璁惧
	uint8_t tcp1_errnum;//tcp1閲嶈繛娆℃暟
	uint8_t heartBeatTime;//mqtt心跳包
	
} WIFI_STR;

typedef struct _TCP_DATA {
	char* data;
	uint16_t len;
} TCP_DATA;

extern WIFI_STR wifi_str;
extern SESSION session[];

#define WIFI_RXBUFF_SIZE	256
#define WIFI_TXBUFF_SIZE	512

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
#define WIFI_DATA_CLASS			8

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


void esp_connect_tcp0 ( void );//杩炴帴TCP0
void esp_connect_tcp1 ( void );//杩炴帴TCP1
void wifi_tcp0_send_data( void );
void wifi_tcp1_send_data( void );
void wifi_mqtt_heart( void );
void mqtt_connect( void );//寤虹珛mqtt杩炴帴
void wifi_init( void );//WIFI初始化
UBaseType_t wifi_mqtt_data_parse( void );//解析mqtt数据

void wifi_data_classification( void );//wifi数据分类

#endif /*__BSP_WIFI__H*/






