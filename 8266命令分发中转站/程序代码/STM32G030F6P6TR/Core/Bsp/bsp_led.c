#include "../Bsp/bsp_led.h"

LED_STR led_str;

void led_init( void ) {
	memset( &led_str, 0, sizeof( LED_STR ) );
}

//LED_NRF闪烁开启
void led_nrf_flicker_on( void ) {
	LED_NRF_OFF();
	led_str.led_mode |= LED_NRF_FLICKER;
}

//LED_NRF闪烁关闭
void led_nrf_flicker_off( void ) {
	LED_NRF_OFF();
	led_str.led_mode &=~ LED_NRF_FLICKER;
}

//LED_NRF设置闪烁速度
int led_nrf_speed_set( uint8_t speed ) {
	if ( speed < 4 ) {
		led_str.led_mode &=~ LED_NRF_FP;
		led_str.led_mode |= speed<<4;
		return 0;
	}
	return -1;
}

//LED_CON闪烁开启
void led_con_flicker_on( void ) {
	LED_CON_OFF();
	led_str.led_mode |= LED_CON_FLICKER;
}

//LED_CON闪烁关闭
void led_con_flicker_off( void ) {
	LED_CON_OFF();
	led_str.led_mode &=~ LED_CON_FLICKER;
}

//LED_CON设置闪烁速度
int led_con_speed_set( uint8_t speed ) {
	if ( speed < 4 ) {
		led_str.led_mode &=~ LED_CON_FP;
		led_str.led_mode |= speed<<6;
		return 0;
	}
	return -1;
}

//定时器中断回调
void led_tim_callback( void ) {
	uint8_t count_t;//00*2:00 01*2:2 10*2:4	11*2:6
	if ( led_str.led_mode & LED_NRF_FLICKER ) {
		led_str.led_nrf_count++;
		count_t = ((led_str.led_mode&LED_NRF_FP)>>2) * 2;
		if ( led_str.led_nrf_count >= count_t ) {
			led_str.led_nrf_count = 0;
			LED_NRF_Toggle();
		}
	}
	if ( led_str.led_mode & LED_CON_FLICKER ) {
		led_str.led_con_count++;
		count_t = ((led_str.led_mode&LED_CON_FP)>>4) * 2;
		if ( led_str.led_con_count >= count_t ) {
			led_str.led_con_count = 0;
			LED_CON_Toggle();
		}
	}
}