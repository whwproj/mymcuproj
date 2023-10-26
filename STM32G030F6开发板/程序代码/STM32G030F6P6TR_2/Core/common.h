#ifndef __COMMON__H
#define __COMMON__H

//1.开机按键注册:		
//2.按键一直注册失败	进入闪烁一直检测
//3.按键停止注册		深度睡眠,程序停止
//4.按键注册成功		正常通信
//5.心跳检测:
//6.心跳检测失败		进入闪烁一直检测
//7.心跳检测成功		正常通信
//8.发送数据失败:		进入闪烁一直检测
//进入闪烁一直检测:	灯闪 deviceReg=1
//正常通信:			灯灭 deviceReg=2

//#define WIFIDEBUG
//#define DEBUG_ENABLE
//#define UNUSE_WIFI
//INCLUDE_uxTaskGetStackHighWaterMark 0

/* LIB ------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>


/*---- Core -------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "spi.h"


/*---- Bsp -------------------------*/
#include "../Bsp/bsp_debug.h"
#include "../Bsp/bsp_nrf.h"
#include "../Bsp/bsp_led.h"
#include "../Bsp/bsp_flash.h"

/*---- Tools -------------------------*/
#include "../Tools/crc.h"


typedef struct _STR {
	uint8_t preKey;//0:未按下 1:按下 按下常亮2s
	uint8_t regSta;//0:不注册 1:正在注册 2:已注册 3:心跳 4:通信成功,等待注册结果
} STR;
	
extern STR str;

#define 	debugTaskSize								256
#define 	executive_taskSize					128
#define 	nrf_control_taskSize				192

extern TaskHandle_t debugTaskHandle;
extern TaskHandle_t executive_taskHandle;
extern TaskHandle_t nrf_control_taskHandle;

void getSnByDeviceId_setClientId( void );
	
#endif /*__COMMON__H*/





