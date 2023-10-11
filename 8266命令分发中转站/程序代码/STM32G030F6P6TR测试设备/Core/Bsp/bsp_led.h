#ifndef __BSP_LED__H
#define __BSP_LED__H

#include "../common.h"
//NRF 红灯
#define LED_NRF_ON() 	HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET )
#define LED_NRF_OFF() 	HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET )
#define LED_NRF_Toggle() 	HAL_GPIO_TogglePin( LED2_GPIO_Port, LED2_Pin )
//CON 蓝灯
#define LED_CON_ON()	HAL_GPIO_WritePin( LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET )
#define LED_CON_OFF()	HAL_GPIO_WritePin( LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET )
#define LED_CON_Toggle() 	HAL_GPIO_TogglePin( LED1_GPIO_Port, LED1_Pin )

#define LED_NRF_FLICKER	(1U<<0)//1:闪烁 0:关闭闪烁
#define LED_CON_FLICKER	(1U<<2)//1:闪烁 0:关闭闪烁
#define LED_NRF_FP	(3U<<4)//闪烁 00:0.5s 01:1s 10:2s 11:3s
#define LED_CON_FP	(3U<<6)//闪烁 00:0.5s 01:1s 10:2s 11:3s

#define LED_SPEED_HEIGHT			0
#define LED_SPEED_MIDDLE_FAST	1
#define LED_SPEED_MIDDLE			2
#define LED_SPEED_SLOW				3

#define EXECUT_TEST						0

typedef struct __LED_STR {
	//bit0:led_nrf bit1:led_con 0:off 1:on
	//bit[2-3]:led_nrf:闪烁时间 bit[4-5]:led_con:闪烁时间 00:0.5s 01:1s 10:2s 11:3s
	uint8_t led_mode;
	uint8_t led_nrf_count;
	uint8_t led_con_count;
} LED_STR;

extern LED_STR led_str;


/*-------------------------------------------------------*/
void led_init( void );
void led_nrf_flicker_on( void );				//LED_NRF闪烁开启
void led_nrf_flicker_off( uint8_t sta );//LED_NRF闪烁关闭,sta指示关闭闪烁后亮灭状态
int  led_nrf_speed_set( uint8_t speed );//LED_NRF设置闪烁速度
void led_con_flicker_on( void );				//LED_CON闪烁开启
void led_con_flicker_off( uint8_t sta );//LED_CON闪烁关闭,sta指示关闭闪烁后亮灭状态
int  led_con_speed_set( uint8_t speed );//LED_CON设置闪烁速度
void led_tim_callback( void );					//定时器中断回调

void led_execut_test( void );
/*-------------------------------------------------------*/


#endif /*__BSP_LED__H*/


