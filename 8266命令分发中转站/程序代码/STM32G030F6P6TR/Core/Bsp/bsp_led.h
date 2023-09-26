#ifndef __BSP_LED__H
#define __BSP_LED__H

#include "../common.h"

#define LED_NRF_ON() 	HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET )
#define LED_NRF_OFF() 	HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET )
#define LED_NRF_Toggle() 	HAL_GPIO_TogglePin( LED2_GPIO_Port, LED2_Pin )

#define LED_CON_ON()	HAL_GPIO_WritePin( LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET )
#define LED_CON_OFF()	HAL_GPIO_WritePin( LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET )
#define LED_CON_Toggle() 	HAL_GPIO_TogglePin( LED1_GPIO_Port, LED1_Pin )


#endif /*__BSP_LED__H*/


