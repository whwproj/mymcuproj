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
#include "tim.h"
//#include "usart.h"
#include "gpio.h"


/*---- Bsp -------------------------*/
//#include "../Bsp/bsp_debug.h"



#define 	debugTaskSize					128


extern TaskHandle_t debugTaskHandle;




#endif /*__COMMON__H*/






