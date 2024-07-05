#include "bsp_debug.h"

#define ds debug_str

DEBUG_STR debug_str;

void debug_init( void ) {
	taskENTER_CRITICAL();
	ds.rxBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	ds.txBuff = pvPortMalloc( DEBUG_BUFF_SIZE );
	taskEXIT_CRITICAL();
	//����DMA���� �������ڿ����ж�
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_IDLE );//ʹ�ܿ����ж�
	__HAL_UART_ENABLE_IT( &huart1, UART_IT_TC );//ʹ�ܷ�������ж�
	HAL_UART_AbortReceive( &huart1 );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );//����DMA����
}


void debug_parse_data( void ) {
	vTaskDelay( 50 );
	HAL_UART_AbortReceive( &huart1 );
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	//�ж�����
	if ( buffCompareToBuff( "����4G��", (char *)ds.rxBuff, strlen("����4G��") ) ) {
		//xTaskNotify( TJC_ControlTaskHandle, 1U<<TJC_CON_WH_LTE_DISCON_TCP232, eSetBits );//����4G��
		
	} else if ( buffCompareToBuff( "����", (char *)ds.rxBuff, strlen("����") ) ) {
		xTaskNotify( pwm_taskHandle, 1U<<TURN_ON, eSetBits );
	
	} else if ( buffCompareToBuff( "�ص�", (char *)ds.rxBuff, strlen("�ص�") ) ) {
		xTaskNotify( pwm_taskHandle, 1U<<TURN_OFF, eSetBits );
	
	} else if ( buffCompareToBuff( "��˸", (char *)ds.rxBuff, strlen("��˸") ) ) {
		xTaskNotify( pwm_taskHandle, 1U<<FLASHING, eSetBits );
		
	} else {
		HAL_UART_Transmit( &huart2, ds.rxBuff, ds.len, 0xffff );
	}
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );//����DMA����
}

BaseType_t buffCompareToBuff( char *buff1, char *buff2, uint16_t len ){
	if ( len == 0 ) return pdFALSE;
	while(len--) {
		if ( *buff1 != *buff2 ) return pdFALSE;
		buff1++;
		buff2++;
	}
	return pdTRUE;
}
