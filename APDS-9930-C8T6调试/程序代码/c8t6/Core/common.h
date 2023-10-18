#ifndef __COMMON__H
#define __COMMON__H

#define DEBUG_ENABLE

// Constants
#define LIGHT_INT_HIGH  1000 // High light level for interrupt
#define LIGHT_INT_LOW   35   // Low light level for interrupt


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
#include "i2c.h"


/*---- Bsp -------------------------*/
#include "../Bsp/bsp_debug.h"
#include "../Bsp/bsp_apds9930.h"

/*---- Tools -------------------------*/


#define 	debugTaskSize								256
extern TaskHandle_t debugTaskHandle;
extern TaskHandle_t APDS9930TaskHandle;

#define LED_ON()	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)
#define LED_OFF()	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)

#endif /*__COMMON__H*/





