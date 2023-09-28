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

#define HTML_HEAD_START "HTTP/1.1 200 OK\r\n"\
	"Content-Type: text/html\r\n"\
	"Content-Length:"
	
#define HTML_HEAD_END 	"\r\n"\
	"Connection: keep-alive\r\n\r\n"
#define HTML_BODY_START "<!DOCTYPE html>"\
"<html>"\
	"<head>"\
		"<meta charset=\"utf-8\">"\
		"<meta name=\"viewport\"content=\"width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=0\">"\
	"</head><body><div style=\"display:block;margin:50px auto; width:180px;\">"
#define HTML_CONTENT_1 "<h5>可为空,为空项则不更改该项原有配置</h5>"\
			"<form action=\"/\" method=\"POST\">"\
			"<input type=\"text\" name=\"wssid\" placeholder=\"wifi ssid\"><br />"\
			"<input type=\"text\" name=\"wpswd\" placeholder=\"wifi password\"><br />"\
			"<input type=\"text\" name=\"tcpurl\" placeholder=\"mqtt host\"><br />"\
			"<input type=\"number\" min=\"1\" max=\"65535\" name=\"tcpport\" placeholder=\"mqtt port\"><br />"\
			"<input type=\"text\" name=\"mqusername\" placeholder=\"mqtt username\"><br />"\
			"<input type=\"text\" name=\"mqpasswd\" placeholder=\"mqtt password\"><br />"\
			"<input type=\"submit\" value=\"保存\"></form>"
#define HTML_CONTENT_2 "<form action=\"/\" method=\"GET\">"\
			"<h3>正在验证请等待...</h3>"\
			"<h4 style=\"color: red;\">1.红色指示灯常亮表示wifi连接失败</h4>"\
			"<h4 style=\"color: red;\">2.蓝色指示灯常亮表示mqtt服务器连接失败</h4>"\
			"<h4 style=\"color: green;\">3.成功连接服务器两个指示灯会灭5s,然后闪烁显示工作状态</h4>"\
			"<input type=\"submit\" value=\"点击重配置\"></form>"
#define HTML_BODY_END "</div></html>"

//连上wifi			
//连接wifi失败	闪烁  1颗灯常亮
//建立tcp成功		灭
//建立tcp失败		

#define STATION_MODE				0
#define STATION_AP_MODE			1

#define WIFI_APTXBUFF_SIZE	1024
#define WIFI_APRXBUFF_SIZE	1024
#define WIFI_TXBUFF_SIZE		128
#define WIFI_RXBUFF_SIZE		128


#define DEFAULT_HEART	60

/*---- wifi task bits start ----------------*/
//*****	wifi_control_task_fun

#define WIFI_PARSE_DATA 		1
#define WIFI_CONNECT_TCP0_	2
#define WIFI_CONNECT_TCP1_	3
#define WIFI_TCP0_SEND			4
#define WIFI_TCP1_SEND			5
#define WIFI_SEND_HEART			6
#define WIFI_SEND_OK				7
#define WIFI_DATA_CLASS			8

#define WIFI_DEVICE_RESET 	0
#define WIFI_UART_IDLE_CALLBACK	10
#define WIFI_STA_AP_MODE_INIT				9
#define WIFI_STATION_MODE_INIT		11


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



typedef struct _SESSION {
	uint8_t onlineSta;//0:offline 1:online
	uint8_t pid;
	uint8_t dir;//main:0 sub1:1...
	uint8_t dataLen;
	uint8_t heart;
	char data[128];
} SESSION;

typedef struct _WIFI_STR {
	uint8_t *txBuff;
	uint8_t *rxBuff;
	uint16_t rxLen;
	uint16_t txLen;
	//uint8_t checkOnlineNum;
	uint8_t tcp0_errnum;//tcp0连接失败次数
	uint8_t tcp1_errnum;//tcp1连接失败次数
	uint8_t wifiMode;
	uint8_t updateLink;//wifi变更,待验证连接成功则存入flash 0:未变更 1:变更
	//uint8_t heartBeatTime;//mqtt心跳包
} WIFI_STR;

typedef struct _TCP_DATA {
	char* data;
	uint16_t len;
} TCP_DATA;

extern WIFI_STR wifi_str;
//extern SESSION session[];


extern WIFI_STR w_str;

//uint32_t wifi_check_online( void );
//void wifi_to_reconfigure( void );
//void wifi_parse_data( void );
//int check_turn_on_time( void );
//uint8_t cmd_main_fun( void );
//uint8_t cmd_sub_1_fun( void );
//uint8_t cmd_sub_2_fun( void );
//uint8_t cmd_sub_3_fun( void );

//void wifi_tcp_send_data( uint8_t pid );//多链路tcp发送数据
//void wifi_tcp1_send_data( void );
//void wifi_mqtt_heart( void );
//void mqtt_connect( void );//寤虹珛mqtt杩炴帴
//void wifi_init( void );//WIFI初始化
//UBaseType_t wifi_mqtt_data_parse( void );//解析mqtt数据


void wifi_reset( void );//wifi复位
void station_mode_init( void );//station模式初始化
void station_and_ap_init( void );//station+AP模式初始化,供用户设置wifi账号密码并存入flash
void wifi_uart_idle_callback( void );//wifi空闲中断回调执行函数


#endif /*__BSP_WIFI__H*/






