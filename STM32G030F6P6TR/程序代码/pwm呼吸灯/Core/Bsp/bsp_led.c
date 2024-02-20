#include "../Bsp/bsp_led.h"

PWM_STR pwm_str = {
	.lightSta=0,
	.maxBtnPer=0,
	.pulse=0,
	.mode=0
};

void pwm_on( void ) {
	
}

void pwm_off( void ) {
	
	
}

void pwm_flicker( void ) {
	
	
}

void pwm_set_speed( uint16_t n ) {
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( n * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );
	pwm_str.lightSta = 1;
}

void led_turn_on ( uint16_t pwmVal ) {
//	uint8_t i;
//	for ( i=0; i<23; i++ ) {
//		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, pwm1[i]);//ўلռࠕ҈
//		vTaskDelay( 100 );
//	}
	//uint16_t pwmVal = 0;
	//if ( pwm_str.lightSta==0 || pwm_str.pulse<=100 ) {
		while ( pwmVal < 1000 ) {
			if ( pwmVal <= 100 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 300 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );
				vTaskDelay(5);
			}
			if ( pwmVal > 300 && pwmVal <= 700 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );
				vTaskDelay(3);
			}
			if ( pwmVal > 700 && pwmVal < 1000 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 1;
		pwm_str.pulse = pwm_str.maxBtnPer;
	//}
}

void led_turn_off ( uint16_t pwmVal ) {
//	uint8_t i;
//	for ( i=23; i>0; i-- ) {
//		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, pwm1[i-1]);//ўلռࠕ҈
//		vTaskDelay( 100 );
//	}
	//uint16_t pwmVal = 1000;
	//if ( pwm_str.lightSta == 1 ) {
		while ( pwmVal ) {
			if ( pwmVal <= 100 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 200 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(5);
			}
			if ( pwmVal > 200 && pwmVal <= 300 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(3);
			}
			if ( pwmVal > 300 && pwmVal <= 1000 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 0;
		pwm_str.pulse = 0;
	//}
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
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 200 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(5);
			}
			if ( pwmVal > 200 && pwmVal <= 300 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(3);
			}
			if ( pwmVal > 300 && pwmVal <= 1000 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
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
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 300 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(5);
			}
			if ( pwmVal > 300 && pwmVal <= 700 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(3);
			}
			if ( pwmVal > 700 && pwmVal < 1000 ) {
				pwmVal++;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 1;
		xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
		xTaskNotifyWait( pdFALSE, 1U<<PWM_FLASHING, &newBits, portMAX_DELAY );
		if ( newBits ) {
			xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
			break;
		}
		
		vTaskDelay(200);
		while ( pwmVal ) {
			if ( pwmVal <= 100 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(7);
			}
			if ( pwmVal > 100 && pwmVal <= 200 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(5);
			}
			if ( pwmVal > 200 && pwmVal <= 300 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(3);
			}
			if ( pwmVal > 300 && pwmVal <= 1000 ) {
				pwmVal--;
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, ( pwmVal * pwm_str.maxBtnPer ) / pwm_str.maxBtnPer );//ўلռࠕ҈
				vTaskDelay(1);
			}
		}
		pwm_str.lightSta = 0;
		xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
		xTaskNotifyWait( pdFALSE, 1U<<PWM_FLASHING, &newBits, portMAX_DELAY );
		if ( newBits ) {
			xTaskNotify( pwm_taskHandle, pdFALSE, eSetBits );
			break;
		}
		
	}
//	if ( pwm_str.lightSta == 1 ) {
//		for ( i=23; i>0; i-- ) {
//			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, pwm1[i-1]);//ўلռࠕ҈
//			vTaskDelay( 100 );
//		}
//	}
//	while( pdTRUE ) {
//		for ( i=0; i<23; i++ ) {
//			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, pwm1[i]);//ўلռࠕ҈
//			vTaskDelay( 100 );
//		}
//		for ( i=23; i>0; i-- ) {
//			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, pwm1[i-1]);//ўلռࠕ҈
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