#include "../Tools/mqtt.h"

typedef unsigned char uchar;
uint16_t MsgId[100];//Message Identifier 在消息质量QoS1或2中总消息条数不能超过100条
//查找未使用过的MsgId,返回前将对应位赋值为(索引+1),置0即清除
static uint8_t getMsgId( void ) {
	uint8_t i = 1;
	while( 1 ) {
		if ( MsgId[ i - 1 ] != i ) {
			MsgId[ i - 1 ] = i;
			return i;
		}
		i++;
	}
}
void clearnMsgId( uint16_t msgId ) {
	MsgId[ msgId - 1 ] = 0;
}
unsigned char GetDataFixedHead(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain)
{
	unsigned char dat = 0;
	dat = (MesType & 0x0f) << 4;
	dat |= (DupFlag & 0x01) << 3;
	dat |= (QosLevel & 0x03) << 1;
	dat |= (Retain & 0x01);
	return dat;
}
void GetDataConnet(uint8_t *buff)//获取连接的数据包正确连接返回20 02 00 00
{
	unsigned int i,len,lennum = 0;
	unsigned char *msg;

	buff[0] = GetDataFixedHead(MQTT_TypeCONNECT,0,0,0);
	buff[2] = 0x00;
	buff[3] = 0x04;
	buff[4] = 'M';
	buff[5] = 'Q';
	buff[6] = 'T';
	buff[7] = 'T';
	buff[8] = 0x04;//协议级别 Protocol Level
	buff[9] = 0 | (MQTT_StaCleanSession << 1) | (MQTT_StaWillFlag << 2)
							| (MQTT_StaWillQoS << 3) | (MQTT_StaWillRetain << 5) 
							|	(MQTT_StaPasswordFlag << 6) |(MQTT_StaUserNameFlag << 7);//连接标志
	buff[10] = MQTT_KeepAlive >> 8;
	buff[11] = ( uint8_t ) MQTT_KeepAlive;
	len = strlen(MQTT_ClientIdentifier);
	buff[12] = len >> 8;
	buff[13] = len;
	msg = (uchar *)MQTT_ClientIdentifier;
	for( i=0; i<len; i++ ) {
		buff[14+i] =  msg[i];
	}
	lennum += len;
	if(MQTT_StaWillFlag)
	{
		len = strlen(MQTT_WillTopic);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = (uchar *)MQTT_WillTopic;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		len = strlen(MQTT_WillMessage);
		buff[12] = len >> 8;
		buff[13] = len;
		lennum += 2;
		msg = (uchar *)MQTT_WillMessage;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	if(MQTT_StaUserNameFlag)
	{
		len = strlen(MQTT_UserName);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = (uchar *)MQTT_UserName;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		
	}
	if(MQTT_StaPasswordFlag)
	{
		len = strlen(MQTT_Password);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = (uchar *)MQTT_Password;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	buff[1] = 13 + lennum - 1;
}
void GetDataDisConnet(unsigned char *buff)//获取断开连接的数据包
{
	buff[0] = 0xe0;
	buff[1] = 0;
}
void GetDataPINGREQ(unsigned char *buff)//心跳请求的数据包成功返回d0 00
{
	buff[0] = 0xc0;
	buff[1] = 0;
}
/*
	成功返回90 0x 00 Num RequestedQoS
*/
//订阅主题的数据包 RequestedQoS:服务质量要求0,1或2
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned char RequestedQoS)
{
	unsigned int i,len = 0;
	uint8_t msgid, idx;
	idx = 0;
	buff[idx++] = 0x82;
	len = strlen(dat);
	buff[idx++] =  2 + /*1 +*/ 2 + len + 1;//(TopicNum(2byte) + [00(1byte)]? + topic长度(2byte) + topic + QoS(1byte))
	msgid = getMsgId();
	buff[idx++] = msgid>>8;
	buff[idx++] = msgid;
	//buff[idx++] = 0x00;
	buff[idx++] = len>>8;
	buff[idx++] = len;
	for(i = 0;i<len;i++)
	{
		buff[idx++] = dat[i];
	}
	buff[ idx ] = RequestedQoS;
}
//封装发布消息的数据包
uint32_t GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)
{
	unsigned int i,len=0;
	//unsigned int lennum=0;
	uint32_t reLen, idx = 0;
	buff[idx++] = GetDataFixedHead(MQTT_TypePUBLISH,dup,qos,retain);
	reLen = strlen(topic) + strlen(msg) + 2;//+2(topic的长度)
	if ( qos != 0 )
		reLen += 2;//qos!=0可变报头格式为:可变总长+topic长度(2byte)+topic+(消息Identifier)(2byte)+有效载荷
	if ( reLen < 128 ) {//1字节
		buff[idx++] = reLen;
	} else if ( reLen>=128 && reLen<16384 ) {//2字节
		buff[idx++] = 0x80 | (reLen & (0x7F<<0));
		buff[idx++] = (reLen & (0x7F<<7)) >> 7;
	} else if ( reLen>=16384 && reLen<2097152 ) {//3字节
		buff[idx++] = 0x80 | (reLen & (0x7F<<0));
		buff[idx++] = 0x80 | (reLen & (0x7F<<7)) >> 7;
		buff[idx++] = (reLen & (0x7F<<14)) >> 14;
	} else {//4字节
		buff[idx++] = 0x80 | (reLen & (0x7F<<0));
		buff[idx++] = 0x80 | (reLen & (0x7F<<7)) >> 7;
		buff[idx++] = 0x80 | (reLen & (0x7F<<14)) >> 14;
		buff[idx++] = (reLen & (0x7F<<21)) >> 21;
	}
	len = strlen(topic);
	buff[idx++] = len>>8;
	buff[idx++] = len;
	for(i = 0;i<len;i++)
	{
		buff[idx++] = topic[i];
	}
	if ( qos == 0 ) {//qos==0可变报头格式为:可变总长+topic长度(2byte)+topic+有效载荷
		
	} else {//qos!=0可变报头格式为:可变总长+topic长度(2byte)+topic+(消息Identifier)(2byte)+有效载荷
		buff[idx++] = 0;
		buff[idx++] = getMsgId();
	}
	len = strlen(msg);
	for(i = 0;i<len;i++)
	{
		buff[idx++] = msg[i];
	}
	return idx;
}
//解析主题,载荷,返回载荷长度
uint32_t mqttParseData( uint8_t *buff, uint8_t *topic, uint8_t *data, uint8_t *qos, uint16_t *msgId) {
	uint32_t d, idx, len = 0;
	uint16_t t, topicLen = 0;
	
	idx = 1;
	len = (buff[idx]&0x7F) << 0;
	if ( buff[idx] & 0x80 ) {//进位
		len |= (buff[++idx]&0x7F) << 7;
		if ( buff[idx] & 0x80 ) {//进位
			len |= (buff[++idx]&0x7F) << 14;
			if ( buff[idx] & 0x80 ){//进位
				len |= buff[++idx] << 21;
			}
		}
	}
	//解析主题
	topicLen = buff[++idx]<<8;
	topicLen |= buff[++idx];
	for ( t=0; t<topicLen; t++ ) {
		topic[t] = buff[++idx];
	} topic[t] = 0;//topic缓存需要存入字符串,末尾补0
	*qos = (buff[0]&0x06) >> 1;
	if ( *qos ) {//QoS!=0
		*msgId = buff[++idx]<<8;
		*msgId |= buff[++idx];
		len = len - 2 - topicLen - 2;//可变总长-主题长度-主题-标识
	} else {
		len = len - 2 - topicLen;//可变总长-主题长度-主题
	}
	//解析载荷
	for ( d=0; d<len; d++ ) {
		data[d] = buff[++idx];
	}
	return len;//有效载荷长度
}




