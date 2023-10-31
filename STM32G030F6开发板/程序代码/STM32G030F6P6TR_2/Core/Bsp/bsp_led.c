#include "../Bsp/bsp_led.h"


void led_mode_0_fun( void ) {
	uint32_t newBits;
	LED_ALL_OFF();
	for ( ;; ) {
		vTaskDelay(100);LED0_ON();
		vTaskDelay(100);LED1_ON();
		vTaskDelay(100);LED0_OFF();LED2_ON();
		vTaskDelay(100);LED1_OFF();LED3_ON();
		vTaskDelay(100);LED2_OFF();LED4_ON();
		vTaskDelay(100);LED3_OFF();LED5_ON();
		vTaskDelay(100);LED4_OFF();LED6_ON();
		vTaskDelay(100);LED5_OFF();LED7_ON();
		vTaskDelay(100);LED6_OFF();
		vTaskDelay(100);LED7_OFF();
		xTaskNotify( executive_taskHandle, pdFALSE, eSetBits );
		xTaskNotifyWait( pdFALSE, pdFALSE, &newBits, 0 );
		if ( newBits != 0 ) {
			xTaskNotify( executive_taskHandle, newBits, eSetBits );
			break;
		}
	}
}

void led_mode_off_fun( void ) {
	LED_ALL_OFF();
}


void key_or_led( uint8_t mode ) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if ( mode == LED_MOD ) {//led
		__HAL_GPIO_EXTI_CLEAR_IT(KEY_Pin);
		HAL_NVIC_DisableIRQ(KEY_EXTI_IRQn);
		GPIO_InitStruct.Pin = KEY_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
		LED4_OFF();
		
	} else if ( mode == KEY_MOD ) {
		GPIO_InitStruct.Pin = KEY_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
		__HAL_GPIO_EXTI_CLEAR_IT(KEY_Pin);
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	}
}











