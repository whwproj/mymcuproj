#ifndef __COMMON__H
#define __COMMON__H

/* Lib ------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/* Inc ------------------------------------------------------------*/
//#include "../Inc/can.h"
//#include "../Inc/tim.h"
//#include "../Inc/rng.h"
#include "../Inc/dma.h"
#include "../Inc/gpio.h"
#include "../Inc/main.h"
//#include "../Inc/spi.h"
#include "../Inc/stm32f1xx_hal_conf.h"
#include "../Inc/stm32f1xx_it.h"
#include "../Inc/usart.h"

/* Tools ----------------------------------------------------------*/
//#include "../Tools/base64.h"
#include "../Tools/cJSON.h"
#include "../Tools/crc.h"
#include "../Tools/mqtt.h"
#include "../Tools/http.h"
//#include "../Tools/other.h"
//#include "../Tools/base64.h"

/* FreeRTOS -------------------------------------------------------*/
#include "../Inc/FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* MbedTLS --------------------------------------------------------*/
//#include "../../MBEDTLS/APP/mbedtls.h"
//#include "../../MBEDTLS/APP/mbedtls_config.h"
//#include "mbedtls/platform.h"

/* Bsp ------------------------------------------------------------*/
#include "../Bsp/bsp_debug.h"
#include "../Bsp/bsp_wifi.h"
//#include "../Bsp/bsp_nrf.h"

/* TaskSizeDefine -------------------------------------------------*/
#define Debug_TaskSize												128
#define wifi_control_taskSize									256
#define wifi_tcp_connect_taskSize							128
//#define NRF_rxTaskSize												256


/* extern declaration ---------------------------------------------*/
extern TaskHandle_t Debug_TaskHandle;
extern TaskHandle_t debug_Task_TaskHandle;
extern TaskHandle_t wifi_control_taskHandle;
extern TaskHandle_t wifi_tcp_connect_taskHandle;
//extern TaskHandle_t NRF_rxTaskHandle;

extern QueueHandle_t TcpData_QueueHandle;//接收数据消息队列
extern QueueHandle_t Mqtt_Ask_And_Heartbeat_QueueHandle;//mqtt应答以及心跳消息队列

//extern QueueHandle_t communication_Mutex;//回话隔离互斥量
extern QueueHandle_t TcpRxCache_Mutex;//获取接收缓存空间信号量
//extern QueueHandle_t TcpTxCache_Mutex;//获取发送缓存空间信号量
extern QueueHandle_t TxSend_Mutex;//发送数据信号量









#endif /*__COMMON__H*/






