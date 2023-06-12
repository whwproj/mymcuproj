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
#include "../Bsp/bsp_wifi.h"
#include "../Bsp/bsp_debug.h"
#include "../Bsp/bsp_flash.h"
#include "../Bsp/bsp_led.h"
#include "../Bsp/bsp_pwm.h"


extern TaskHandle_t usart_wifi_TaskHandle;
extern TaskHandle_t usart_debug_TaskHandle;
extern TaskHandle_t led_taskHandle;
extern TaskHandle_t cmd_handle_taskHandle;
extern TaskHandle_t pwm_taskHandle;

extern QueueHandle_t cmd_queueHandle;
extern QueueHandle_t send_The_Right_Mutex;





#endif /*__COMMON__H*/






