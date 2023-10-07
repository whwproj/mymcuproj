#ifndef	__MQTT__H
#define	__MQTT__H

#include "../common.h"

#define		MQTT_TypeCONNECT							1//请求连接  
#define		MQTT_TypeCONNACK							2//请求应答  
#define		MQTT_TypePUBLISH							3//发布消息  
#define		MQTT_TypePUBACK								4//发布应答  
#define		MQTT_TypePUBREC								5//发布已接收，保证传递1  
#define		MQTT_TypePUBREL								6//发布释放，保证传递2  
#define		MQTT_TypePUBCOMP							7//发布完成，保证传递3  
#define		MQTT_TypeSUBSCRIBE						8//订阅请求  
#define		MQTT_TypeSUBACK								9//订阅应答  
#define		MQTT_TypeUNSUBSCRIBE					10//取消订阅  
#define		MQTT_TypeUNSUBACK							11//取消订阅应答  
#define		MQTT_TypePINGREQ							12//ping请求  
#define		MQTT_TypePINGRESP							13//ping响应  
#define		MQTT_TypeDISCONNECT 					14//断开连接  
 
#define		MQTT_StaCleanSession					1	//清理会话 
#define		MQTT_StaWillFlag							0	//遗嘱标志
#define		MQTT_StaWillQoS								0	//遗嘱QoS连接标志的第4和第3位。
#define		MQTT_StaWillRetain						0	//遗嘱保留
#define		MQTT_StaUserNameFlag					1	//用户名标志 User Name Flag
#define		MQTT_StaPasswordFlag					1	//密码标志 Password Flag
#define		MQTT_KeepAlive								300
#define		MQTT_ClientIdentifier  	udata.duid			//"mqttx_b9cb3b0c"	//客户端标识符 Client Identifier
#define		MQTT_WillTopic					""				//遗嘱主题 Will Topic
#define		MQTT_WillMessage				""				//遗嘱消息 Will Message
#define		MQTT_UserName						udata.mqusername	//用户名 User Name
#define		MQTT_Password						udata.mqpasswd		//密码 Password
#define		MQTT_SUB1Topic					"message"				//订阅主题1
#define		MQTT_SUB2Topic					"config"				//订阅主题2
#define		MQTT_PUBTopic						"reply"					//发布主题


unsigned char GetDataFixedHead(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain);
uint32_t GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);//获取发布消息的数据包		 	
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned char RequestedQoS);//订阅主题的数据包 RequestedQoS:服务质量要求0,1或2
void GetDataDisConnet(unsigned char *buff);//获取断开连接的数据包
void GetDataConnet(unsigned char *buff);//获取连接的数据包正确连接返回20 02 00 00
void GetDataPINGREQ(unsigned char *buff);//心跳请求的数据包成功返回d0 00
uint32_t mqttParseData( uint8_t *buff, uint8_t *topic, uint8_t *data, uint8_t *qos, uint16_t *msgId);//解析主题,载荷,返回载荷长度
void clearnMsgId( uint16_t msgId );


#endif /*__MQTT__H*/















