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
#include "../Bsp/bsp_debug.h"
#include "../Bsp/bsp_wifi.h"
#include "../Bsp/bsp_nrf.h"
#include "../Bsp/bsp_led.h"
#include "../Bsp/bsp_flash.h"
#include "../Bsp/bsp_json.h"
#include "../Bsp/bsp_data.h"
#include "../Bsp/bsp_w25qxx.h"

/*---- Tools -------------------------*/
#include "../Tools/tool_url.h"
#include "../Tools/cJSON.h"
#include "../Tools/mqtt.h"
#include "../Tools/crc.h"


#define 	debugTaskSize								128
#define 	wifi_control_taskSize				192
#define 	nrf_control_taskSize				192
#define		time_taskSize								64

extern TaskHandle_t debugTaskHandle;
extern TaskHandle_t wifi_control_taskHandle;
//extern TaskHandle_t data_task_handle;
extern TaskHandle_t nrf_control_taskHandle;
extern QueueHandle_t wifi_data_handle;
extern TaskHandle_t time_task_handle;


void getSnByDeviceId_setClientId( void );//通过唯一设备ID获取SN
#endif /*__COMMON__H*/





