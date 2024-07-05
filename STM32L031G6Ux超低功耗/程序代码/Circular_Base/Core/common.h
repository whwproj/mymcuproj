#ifndef __COMMON__H
#define __COMMON__H

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
#include "gpio.h"
#include "spi.h"
#include "i2c.h"


/*---- Bsp -------------------------*/
#include "../Bsp/bsp_nrf.h"
#include "../Bsp/bsp_apds9930.h"
#include "../Bsp/bsp_w25qxx.h"
#include "../Bsp/bsp_wkup.h"
#include "../Bsp/bsp_oled.h"
#include "../Bsp/bsp_olde_font.h"


#define 	debugTaskSize					128


extern TaskHandle_t debugTaskHandle;




#endif /*__COMMON__H*/






