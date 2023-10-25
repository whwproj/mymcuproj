#ifndef __BSP_LED__H
#define __BSP_LED__H

#include "../common.h"

#define LED0_ON()			HAL_GPIO_WritePin( LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET )
#define LED0_OFF()		HAL_GPIO_WritePin( LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET )
#define LED0_TOGGLE()	HAL_GPIO_TogglePin( LED0_GPIO_Port, LED0_Pin )

#define LED1_ON()			HAL_GPIO_WritePin( LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET )
#define LED1_OFF()		HAL_GPIO_WritePin( LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET )
#define LED1_TOGGLE()	HAL_GPIO_TogglePin( LED1_GPIO_Port, LED1_Pin )

#define LED2_ON()			HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET )
#define LED2_OFF()		HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET )
#define LED2_TOGGLE()	HAL_GPIO_TogglePin( LED2_GPIO_Port, LED2_Pin )

#define LED3_ON()			HAL_GPIO_WritePin( LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET )
#define LED3_OFF()		HAL_GPIO_WritePin( LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET )
#define LED3_TOGGLE()	HAL_GPIO_TogglePin( LED3_GPIO_Port, LED3_Pin )

#define LED4_ON()			//HAL_GPIO_WritePin( LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET )
#define LED4_OFF()		//HAL_GPIO_WritePin( LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET )
#define LED4_TOGGLE()	//HAL_GPIO_TogglePin( LED4_GPIO_Port, LED4_Pin )

#define LED5_ON()			HAL_GPIO_WritePin( LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET )
#define LED5_OFF()		HAL_GPIO_WritePin( LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET )
#define LED5_TOGGLE()	HAL_GPIO_TogglePin( LED5_GPIO_Port, LED5_Pin )

#define LED6_ON()			HAL_GPIO_WritePin( LED6_GPIO_Port, LED6_Pin, GPIO_PIN_RESET )
#define LED6_OFF()		HAL_GPIO_WritePin( LED6_GPIO_Port, LED6_Pin, GPIO_PIN_SET )
#define LED6_TOGGLE()	HAL_GPIO_TogglePin( LED6_GPIO_Port, LED6_Pin )

#define LED7_ON()			HAL_GPIO_WritePin( LED7_GPIO_Port, LED7_Pin, GPIO_PIN_RESET )
#define LED7_OFF()		HAL_GPIO_WritePin( LED7_GPIO_Port, LED7_Pin, GPIO_PIN_SET )
#define LED7_TOGGLE()	HAL_GPIO_TogglePin( LED7_GPIO_Port, LED7_Pin )

#define LED_ALL_ON()	{\
	LED0_ON();LED1_ON();LED2_ON();LED3_ON();\
	/*LED4_ON();*/LED5_ON();LED6_ON();LED7_ON();\
}

#define LED_ALL_OFF()	{\
	LED0_OFF();LED1_OFF();LED2_OFF();LED3_OFF();\
	/*LED4_OFF();*/LED5_OFF();LED6_OFF();LED7_OFF();\
}


#define LED_MODE_OFF		0	
#define LED_MODE_0			1
#define LED_MODE_1			2
#define LED_MODE_2			3
#define LED_MODE_3			4

void led_mode_0_fun( void );
void led_mode_off_fun( void );

#endif /*__BSP_LED__H*/


