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
		vTaskDelay(100);LED5_OFF();/*LED7_ON();*/
		vTaskDelay(100);LED6_OFF();
		/*vTaskDelay(100);LED7_OFF();*/
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

