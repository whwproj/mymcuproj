#ifndef __BSP_LED__H
#define __BSP_LED__H

#include "../common.h"

#define HX_ON()		HAL_GPIO_WritePin( HXCTL_GPIO_Port, HXCTL_Pin, GPIO_PIN_SET );
#define HX_OFF()		HAL_GPIO_WritePin( HXCTL_GPIO_Port, HXCTL_Pin, GPIO_PIN_RESET );

#define PWM_FLASHING		0
#define LED_ON						1
#define LED_OFF						2
#define POWER_ON				3
#define POWER_OFF				4

typedef struct _PWM_STR {
	uint8_t lightSta;//0灭 1亮
	uint16_t maxBtnPer;
} PWM_STR;
extern PWM_STR pwm_str;

void pwm_on( void );
void pwm_off( void );
void pwm_flicker( void );
void pwm_set_speed( uint16_t n );

void pwm_flashing ( void );
void led_turn_off ( void );
void led_turn_on ( void );
	
#endif /*__BSP_LED__H*/

