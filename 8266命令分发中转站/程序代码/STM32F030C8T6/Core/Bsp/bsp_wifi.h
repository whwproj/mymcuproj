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

#define ENABLE_IT	0x80
#define NOT_STR		0x40



#define DEFAULT_HEART	60

/*---- wifi task bits start ----------------*/

//*****	wifi_control_task_fun
#define WIFI_DEVICE_RESET 				0
#define WIFI_UART_IDLE_CALLBACK		1
#define WIFI_STA_AP_MODE_INIT			2
#define WIFI_STATION_MODE_INIT		3
#define PARSE_JSON_DATA						8

#define WIFI_SEND_HEART						0
#define WIFI_SEND_MQTT						1


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

typedef struct ___SESSION {
	//uint8_t deviceId;
	uint8_t len;
	//uint16_t code;
	char data[128];
} _SESSION;

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
	
	uint8_t sendLock;//mqtt心跳,mqtt数据不可同时发送,0:需等待 1:可发送
	uint8_t mqttSta;//0:未连接 1:建立连接订阅主题完毕
	uint8_t heartBeatTime;//mqtt心跳包
	
	_SESSION sendSession[3];
	_SESSION receiveSession[3];
} WIFI_STR;
extern WIFI_STR w_str;

typedef struct __DATA_STR {
	uint16_t code;
	uint8_t nrfid;
	char* data;
} DATA_STR;
extern DATA_STR data_str;


void wifi_reset( void );//wifi复位
void station_mode_init( void );//station模式初始化
void station_and_ap_init( void );//station+AP模式初始化,供用户设置wifi账号密码并存入flash
void wifi_uart_idle_callback( void );//wifi空闲中断回调执行函数
void send_mqtt_heart_isr( void );//发送心跳ISR
void send_mqtt_heart( void );//发送心跳
//void send_device_not_register( void );//回复设备未注册
//int send_device_not_online( void );//回复设备不在线
//void send_forward_success( void );//转发成功
//void push_data_fun( char *data );//推送数据

void parse_wifi_data_fun( void );//解析wifi数据

//void device_not_register_addSendList( uint8_t deviceId, uint16_t code );//回复设备未注册
//int device_not_online_addSendList( uint8_t deviceId, uint16_t code );//回复设备不在线
//void send_forward_success( uint8_t deviceId, uint16_t code );//转发成功

void wifi_msg_add_SendList( uint8_t deviceId, uint16_t code, char* p_dat, int errCode );//添加待发送的wifi消息
void wifi_send_mqtt( void );
#endif /*__BSP_WIFI__H*/






