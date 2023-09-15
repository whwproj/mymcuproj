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
//#include "../Tools/cJSON.h"
//#include "../Tools/crc.h"
//#include "../Tools/mqtt.h"
//#include "../Tools/http.h"
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












#endif /*__COMMON__H*/






