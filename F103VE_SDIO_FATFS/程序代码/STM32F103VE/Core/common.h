#ifndef __COMMON__H
#define __COMMON__H


//#define WIFIDEBUG
//#define NRF_DEBUG
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
//#include "spi.h"
#include "sdio.h"
#include "diskio.h"
#include "fatfs.h"

/*---- Bsp -------------------------*/
//#include "../Bsp/bsp_wifi.h"
//#include "../Bsp/bsp_nrf.h"
//#include "../Bsp/bsp_led.h"
//#include "../Bsp/bsp_flash.h"
//#include "../Bsp/bsp_json.h"
//#include "../Bsp/bsp_w25qxx.h"
#include "../Bsp/bsp_sdio.h"
#include "../Bsp/bsp_fatfs.h"
#include "../Bsp/bsp_debug.h"

/*---- Tools -------------------------*/
//#include "../Tools/tool_url.h"
//#include "../Tools/cJSON.h"
//#include "../Tools/mqtt.h"
//#include "../Tools/crc.h"

//#define 	wifi_control_taskSize				192
//#define 	nrf_control_taskSize				100
//#define 	wifi_send_taskSize				  100

typedef struct __USER_STR {
	uint8_t sdInit;//0:默认 1:卡就绪
	uint8_t transferStar;//0:默认 1:正在传输
	uint32_t size;
	uint32_t wSize;
	char name[32];
	
} _USER_STR;
extern _USER_STR u_str;

extern TaskHandle_t debugTaskHandle;		

//extern TaskHandle_t nrf_control_taskHandle;
//extern TaskHandle_t wifi_send_taskHandle;

//extern QueueHandle_t wifi_send_cache_handle;
//extern QueueHandle_t wifi_receive_cache_handle;
//extern QueueHandle_t wifi_using_lock_handle;//互斥量

//void getSnByDeviceId_setClientId( void );//通过唯一设备ID获取SN
//void restore_factory_setting ( void );
//void Delay_Us(uint16_t t);
//void Delay_Ms(uint32_t t);
//void Delay_S(uint32_t t);

#endif /*__COMMON__H*/





