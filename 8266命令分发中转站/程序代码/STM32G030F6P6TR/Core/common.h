#ifndef __COMMON__H
#define __COMMON__H

//#define WIFIDEBUG
//#define DEBUG_ENABLE

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


#define 	debugTaskSize								256
#define 	wifi_control_taskSize				128
#define 	wifi_tcp_connect_taskSize		128

extern TaskHandle_t debugTaskHandle;
extern TaskHandle_t wifi_control_taskHandle;
extern TaskHandle_t wifi_tcp_connect_taskHandle;

#endif /*__COMMON__H*/





