#include "../Bsp/bsp_pwm.h"

uint16_t pwm1[] = {0,10,13,17,22,30,40,52,69,91,121,160,211,278,367,485,640,844,1114,1470,1940,2560,1940};
//uint16_t pwm2[] = {1940,2560,1940,1470,1114,844,640,485,367,278,211,160,121,91,69,52,40,30,22,17,13,10};
	
PWM_STR pwm_str;

void pwm_turn_on ( void ) {
	uint8_t i;
	for ( i=0; i<23; i++ ) {
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i]);//修改占空比
		vTaskDelay( 100 );
	}
}

void pwm_turn_off ( void ) {
	uint8_t i;
	for ( i=23; i>0; i-- ) {
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i-1]);//修改占空比
		vTaskDelay( 100 );
	}
}

void pwm_flashing ( void ) {
	uint32_t newBits;
	uint8_t i;
	
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);//修改占空比
	vTaskDelay( 3000 );
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 1940);//修改占空比
	
	if ( pwm_str.lightSta == 1 ) {
		for ( i=23; i>0; i-- ) {
			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i-1]);//修改占空比
			vTaskDelay( 100 );
		}
	}
	while( pdTRUE ) {
		for ( i=0; i<23; i++ ) {
			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i]);//修改占空比
			vTaskDelay( 100 );
		}
		for ( i=23; i>0; i-- ) {
			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i-1]);//修改占空比
			vTaskDelay( 100 );
		}
//		xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
//		xTaskNotifyWait( pdFALSE, 1U<<FLASHING, &newBits, portMAX_DELAY );
//		if ( newBits ) {
//			pwm_str.lightSta = 0;
//			break;
//		}
	}
}


