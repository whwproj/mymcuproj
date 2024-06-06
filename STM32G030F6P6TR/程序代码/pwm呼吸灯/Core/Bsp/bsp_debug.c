#include "../Bsp/bsp_debug.h"

#define ds debug_str

DEBUG_STR debug_str;

void debug_init( void ) {
	taskENTER_CRITICAL();
	ds.rxBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.txBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_IDLE );
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_TC );
	HAL_UART_AbortReceive( &huart1 );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );
}


void debug_parse_data_fun( void ) {
	vTaskDelay( 50 );
	HAL_UART_AbortReceive( &huart1 );
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	if ( strstr( (char *)ds.rxBuff, "led_on" ) != NULL ) {
		xTaskNotify( pwm_taskHandle, 1U<<LED_ON, eSetBits );
		printf("led_on ok\r\n");
	} else if ( strstr( (char *)ds.rxBuff, "led_off" ) != NULL ) {
		xTaskNotify( pwm_taskHandle, 1U<<LED_OFF, eSetBits );
		printf("led_off ok\r\n");
	}  else if ( strstr( (char *)ds.rxBuff, "power_on" ) != NULL ) {
		xTaskNotify( pwm_taskHandle, 1U<<POWER_ON, eSetBits );
		printf("power_on ok\r\n");
	} else if ( strstr( (char *)ds.rxBuff, "power_off" ) != NULL ) {
		xTaskNotify( pwm_taskHandle, 1U<<POWER_OFF, eSetBits );
		printf("power_off ok\r\n");
	}  else if ( strstr( (char *)ds.rxBuff, "pwm_flashing" ) != NULL ) {
		xTaskNotify( pwm_taskHandle, 1U<<PWM_FLASHING, eSetBits );
		printf("pwm_flashing ok\r\n");
	}
	memset( ds.rxBuff, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );
}


