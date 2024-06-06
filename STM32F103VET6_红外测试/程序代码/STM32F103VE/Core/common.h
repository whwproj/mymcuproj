#ifndef __COMMON__H
#define __COMMON__H

/* Lib ------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/* Inc ------------------------------------------------------------*/
//#include "../Inc/can.h"
#include "../Inc/tim.h"
//#include "../Inc/rng.h"
#include "../Inc/dma.h"
#include "../Inc/gpio.h"
#include "../Inc/main.h"
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

/* MbedTLS --------------------------------------------------------*/
//#include "../../MBEDTLS/APP/mbedtls.h"
//#include "../../MBEDTLS/APP/mbedtls_config.h"
//#include "mbedtls/platform.h"

/* Bsp ------------------------------------------------------------*/
#include "../Bsp/bsp_debug.h"
#include "../Bsp/bsp_ir.h"

/* TaskSizeDefine -------------------------------------------------*/


/* extern declaration ---------------------------------------------*/
#define LEDG_ON()		HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, GPIO_PIN_RESET)
#define LEDG_OFF()	HAL_GPIO_WritePin(LEDG_GPIO_Port, LEDG_Pin, GPIO_PIN_SET)
#define IR_IsH() 			HAL_GPIO_ReadPin(IR_GPIO_Port, IR_Pin)



#endif /*__COMMON__H*/






