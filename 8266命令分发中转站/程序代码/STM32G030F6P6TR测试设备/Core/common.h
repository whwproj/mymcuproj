#ifndef __COMMON__H
#define __COMMON__H


//#define WIFIDEBUG
#define DEBUG_ENABLE
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


#define 	debugTaskSize								256
#define 	executive_taskSize					128
#define 	nrf_control_taskSize				192

extern TaskHandle_t debugTaskHandle;
extern TaskHandle_t executive_taskHandle;
extern TaskHandle_t nrf_control_taskHandle;

void getSnByDeviceId_setClientId( void );
	
#endif /*__COMMON__H*/





