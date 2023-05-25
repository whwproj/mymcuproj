#ifndef __COMMON__H
#define __COMMON__H

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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"


/*---- Bsp -------------------------*/
#include "bsp_wifi.h"
#include "bsp_debug.h"


extern TaskHandle_t usart_wifi_TaskHandle;
extern TaskHandle_t usart_debug_TaskHandle;








#endif /*__COMMON__H*/






