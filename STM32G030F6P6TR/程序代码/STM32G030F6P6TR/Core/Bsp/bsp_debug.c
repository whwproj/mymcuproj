#include "../Bsp/bsp_debug.h"

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


void debug_parse_data_fun( void ) {
	vTaskDelay( 50 );
	HAL_UART_AbortReceive( &huart1 );
	ds.len = DEBUG_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	//�ж�����
	//if ( buffCompareToBuff( "����4G��", (char *)ds.rxBuff, strlen("����4G��") ) ) {
	if ( strstr( (char *)ds.rxBuff, "����4G��" ) != NULL ) {
		//xTaskNotify( TJC_ControlTaskHandle, 1U<<TJC_CON_WH_LTE_DISCON_TCP232, eSetBits );//����4G��
		
	} else if ( strstr( (char *)ds.rxBuff, "����" ) != NULL ) {
		//xTaskNotify( pwm_taskHandle, 1U<<TURN_ON, eSetBits );
	
	} else if ( strstr( (char *)ds.rxBuff, "�ص�" ) != NULL ) {
		//xTaskNotify( pwm_taskHandle, 1U<<TURN_OFF, eSetBits );
	
	} else if ( strstr( (char *)ds.rxBuff, "��˸" ) != NULL ) {
		//xTaskNotify( pwm_taskHandle, 1U<<FLASHING, eSetBits );
		
	} else if ( strstr( (char *)ds.rxBuff, "clear reg" ) != NULL ) {
		SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xf0 );//0xFF��ָ��
		printf("clear reg ok\r\n");

	} else if ( strstr( (char *)ds.rxBuff, "clear fifo" ) != NULL ) {
		SPI_RW_Reg( FLUSH_RX,NOP );
		printf("clear fifo ok\r\n");
		
	} else if ( strstr( (char *)ds.rxBuff, "reg" ) != NULL ) {
		uint8_t sta = SPI_RW_Reg(NRF_READ_REG + STATUS, 0xff );//0xFF��ָ��
		printf("sta: %02X\r\n", sta);
		
	} else if ( strstr( (char *)ds.rxBuff, "gpio" ) != NULL ) {
		uint8_t temp = HAL_GPIO_ReadPin( NRF_IRQ_GPIO_Port, NRF_IRQ_Pin );
		printf("temp: %d\r\n", temp);
	
	}
	memset( ds.rxBuff, 0, DEBUG_BUFF_SIZE );
	HAL_UART_Receive_DMA( &huart1, ds.rxBuff, DEBUG_BUFF_SIZE );//����DMA����
}

//BaseType_t buffCompareToBuff( char *buff1, char *buff2, uint16_t len ){
//	if ( len == 0 ) return pdFALSE;
//	while(len--) {
//		if ( *buff1 != *buff2 ) return pdFALSE;
//		buff1++;
//		buff2++;
//	}
//	return pdTRUE;
//}
