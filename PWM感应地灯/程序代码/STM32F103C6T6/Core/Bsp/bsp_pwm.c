#include "../Bsp/bsp_pwm.h"

//uint16_t pwm1[] = {0,10,13,17,22,30,40,52,69,91,121,160,211,278,367,485,640,844,1114,1470,1940,2560,1940};
//uint16_t pwm2[] = {1940,2560,1940,1470,1114,844,640,485,367,278,211,160,121,91,69,52,40,30,22,17,13,10};
	
PWM_STR pwm_str;

void pwm_turn_on ( void ) {
//	uint8_t i;
//	for ( i=0; i<23; i++ ) {
//		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i]);//修改占空比
//		vTaskDelay( 100 );
//	}
	uint16_t pwmVal = 0;
	if ( pwm_str.lightSta == 0 ) {
		while ( pwmVal < 1000 ) {
			if ( pwmVal <= 100 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 300 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(5);
			}
			if ( pwmVal > 300 && pwmVal <= 700 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(3);
			}
			if ( pwmVal > 700 && pwmVal < 1000 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 1;
	}
}

void pwm_turn_off ( void ) {
//	uint8_t i;
//	for ( i=23; i>0; i-- ) {
//		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i-1]);//修改占空比
//		vTaskDelay( 100 );
//	}
	uint16_t pwmVal = 1000;
	if ( pwm_str.lightSta == 1 ) {
		while ( pwmVal ) {
			if ( pwmVal <= 100 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 200 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(5);
			}
			if ( pwmVal > 200 && pwmVal <= 300 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(3);
			}
			if ( pwmVal > 300 && pwmVal <= 1000 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 0;
	}
}

void pwm_flashing ( void ) {
	uint32_t newBits;
	//uint8_t i;
	uint16_t pwmVal;
	if ( pwm_str.lightSta == 1 ) {
		pwmVal = 1000;
		while ( pwmVal ) {
			if ( pwmVal <= 100 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 200 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(5);
			}
			if ( pwmVal > 200 && pwmVal <= 300 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(3);
			}
			if ( pwmVal > 300 && pwmVal <= 1000 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 0;
	}
	
	pwmVal = 0;
	for ( ;; ) {
		vTaskDelay(200);
		while ( pwmVal < 1000 ) {
			if ( pwmVal <= 100 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 300 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(5);
			}
			if ( pwmVal > 300 && pwmVal <= 700 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(3);
			}
			if ( pwmVal > 700 && pwmVal < 1000 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 1;
		xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
		xTaskNotifyWait( pdFALSE, 1U<<FLASHING, &newBits, portMAX_DELAY );
		if ( newBits ) {
			xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
			break;
		}
		
		vTaskDelay(200);
		while ( pwmVal ) {
			if ( pwmVal <= 100 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 200 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(5);
			}
			if ( pwmVal > 200 && pwmVal <= 300 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(3);
			}
			if ( pwmVal > 300 && pwmVal <= 1000 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, ( pwmVal * pwm_str.maxBtnPer ) / 100 );//修改占空比
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 0;
		xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
		xTaskNotifyWait( pdFALSE, 1U<<FLASHING, &newBits, portMAX_DELAY );
		if ( newBits ) {
			xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
			break;
		}
		
	}
//	if ( pwm_str.lightSta == 1 ) {
//		for ( i=23; i>0; i-- ) {
//			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i-1]);//修改占空比
//			vTaskDelay( 100 );
//		}
//	}
//	while( pdTRUE ) {
//		for ( i=0; i<23; i++ ) {
//			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i]);//修改占空比
//			vTaskDelay( 100 );
//		}
//		for ( i=23; i>0; i-- ) {
//			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, pwm1[i-1]);//修改占空比
//			vTaskDelay( 100 );
//		}
////		xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
////		xTaskNotifyWait( pdFALSE, 1U<<FLASHING, &newBits, portMAX_DELAY );
////		if ( newBits ) {
////			pwm_str.lightSta = 0;
////			break;
////		}
//	}
}


