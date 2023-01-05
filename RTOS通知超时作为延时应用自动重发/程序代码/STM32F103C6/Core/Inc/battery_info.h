#ifndef __BATTERY_INFO__H
#define __BATTERY_INFO__H

#include "main.h"

#define BATT_SIZE							sizeof(BATT)


void battParamInit( void );

//电池版本号
typedef struct {
	char hw[12];
	char boot[7];
	char app[7];
	char lastapp[7];//最新app
} battver;

//电池充电信息
typedef struct {//size:800
	//char sn[15];  //电池SN
	uint8_t power;	 //电量
	float temperature;//温度
	uint8_t cycles;//循环次数
	uint16_t capacity;//电池容量
	float batteryVoltage;//电池电压
	float cellVoltage;//电池电压
	//char imgurl[20];//电池图片
	
} chargerBatInfo;

//充电器层信息
typedef struct {
	uint8_t syncChargeNum;//同时充电数量
	uint8_t chargeMode;//充电模式
	battver version;//电池版本信息
	chargerBatInfo cbInfo[13];//电池充电信息
} chargeLayer;


typedef struct BATTERYINFO {

	char sn[20];//电池柜SN
	battver version;//柜体版本信息
	
	uint8_t serverIp[4];//服务器ip地址
	uint16_t serverPort;//服务器端口号
	
	chargeLayer cLayer[6];//充电器层信息//size:800

} BATT;


extern BATT batt;





#endif

