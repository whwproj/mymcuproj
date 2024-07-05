#ifndef __COMMON__H
#define __COMMON__H


//#define WIFIDEBUG
#define NRF_DEBUG
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
#include "../Bsp/bsp_wifi.h"
#include "../Bsp/bsp_nrf.h"
#include "../Bsp/bsp_led.h"
#include "../Bsp/bsp_flash.h"
#include "../Bsp/bsp_json.h"
#include "../Bsp/bsp_w25qxx.h"

/*---- Tools -------------------------*/
#include "../Tools/tool_url.h"
#include "../Tools/cJSON.h"
#include "../Tools/mqtt.h"
#include "../Tools/crc.h"

#define 	wifi_control_taskSize				192
#define 	nrf_control_taskSize				100
#define 	wifi_send_taskSize				  100

extern TaskHandle_t wifi_control_taskHandle;
extern TaskHandle_t nrf_control_taskHandle;
extern TaskHandle_t wifi_send_taskHandle;

extern QueueHandle_t wifi_send_cache_handle;
extern QueueHandle_t wifi_receive_cache_handle;
extern QueueHandle_t wifi_using_lock_handle;//互斥量

void getSnByDeviceId_setClientId( void );//通过唯一设备ID获取SN
#endif /*__COMMON__H*/





