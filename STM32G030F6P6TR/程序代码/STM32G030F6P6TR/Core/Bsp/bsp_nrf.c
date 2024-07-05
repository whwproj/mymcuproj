#include "../Bsp/bsp_nrf.h"

uint8_t Long;
//uint8_t TX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01}; //���ص�ַ 
//uint8_t RX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01}; //���յ�ַ
uint8_t TX_ADDRESS[5] = {0xA3,0xA3,0xA3,0xA3,0xA3}; //���ص�ַ 
uint8_t RX_ADDRESS[5] = {0xA3,0xA3,0xA3,0xA3,0xA3}; //���յ�ַ
uint8_t tx_buf[]={"��������G030F6P6����Ϣ\r\n"};
uint8_t rx_buf[224];

void NRF2_Init(void);
	
uint8_t SPI_RW_Reg( uint8_t reg, uint8_t value ) {//��д�Ĵ���
	uint8_t status; 
	CSN_Low();
	HAL_SPI_Transmit( &hspi1, &reg, 1, 100 );
	HAL_SPI_TransmitReceive( &hspi1, &value, &status, 1, 100 );
	CSN_High();
	return(status);
} 

uint8_t SPI_Write_Buf( uint8_t reg, uint8_t *pBuf, uint8_t len ) {//���ֽ�д��
	uint8_t status; 
	CSN_Low();
	HAL_SPI_TransmitReceive( &hspi1, &reg, &status, 1, 100 );
	HAL_SPI_Transmit( &hspi1, pBuf, len, 100 );
	CSN_High();
	return(status);  
}  

uint8_t SPI_Read_Buf( uint8_t reg, uint8_t *pBuf, uint8_t len ) {//���ֽڶ�ȡ
	uint8_t status; 
	CSN_Low();
	HAL_SPI_TransmitReceive( &hspi1, &reg, &status, 1, 100 );
	HAL_SPI_Receive( &hspi1, pBuf, len, 100);
	CSN_High();
	return(status); // return nRF24L01 status uchar 
}


uint8_t Nrf24l01_Init( NRF24L01_TypeDef *nrf ) {
	uint8_t i;
	CE_High();
	vTaskDelay(100);
	CE_Low();
	vTaskDelay(100);
	SPI_RW_Reg( FLUSH_TX, NOP );//���FIFO
	SPI_RW_Reg( FLUSH_RX, NOP );//���FIFO
	SPI_RW_Reg( NRF_WRITE_REG + CONFIG, nrf->CONFIG_ );//CONFIG ����ģʽ���üĴ���
	i = SPI_RW_Reg( NRF_READ_REG+CONFIG, NOP );
	SPI_RW_Reg( NRF_WRITE_REG + EN_AA, nrf->EN_AA_ );//EN_AA ʹ���Զ�ȷ�Ϲ���(Enhanced ShockBurst)
	SPI_RW_Reg( NRF_WRITE_REG + EN_RXADDR, nrf->EN_RXADDR_ );//EN_RXADDR ʹ�ܽ���ͨ��(0~5)
	SPI_RW_Reg( NRF_WRITE_REG + SETUP_AW, nrf->SETUP_AW_ );//SETUP_AW ���õ�ַ���[3-5]byte
	SPI_RW_Reg( NRF_WRITE_REG + SETUP_RETR, nrf->SETUP_RETR_ );//SETUP_RETR �����Զ��ش�
	SPI_RW_Reg( NRF_WRITE_REG + RF_CH, nrf->RF_CH_ );//RF_CH ��ƵƵ��
	SPI_RW_Reg( NRF_WRITE_REG + RF_SETUP, nrf->RF_SETUP_ );//RF_SETUP ��Ƶ���üĴ���
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, nrf->STATUS_ );//STATUS �ж�״̬�Ĵ���
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P0, nrf->RX_ADDR_P0_, nrf->SETUP_AW_+2 );//��������ͨ��0��ַ
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P1, nrf->RX_ADDR_P1_, nrf->SETUP_AW_+2 );//��������ͨ��1��ַ
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P2, nrf->RX_ADDR_P2_, nrf->SETUP_AW_+2 );//��������ͨ��2��ַ
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P3, nrf->RX_ADDR_P3_, nrf->SETUP_AW_+2 );//��������ͨ��3��ַ
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P4, nrf->RX_ADDR_P4_, nrf->SETUP_AW_+2 );//��������ͨ��4��ַ
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P5, nrf->RX_ADDR_P5_, nrf->SETUP_AW_+2 );//��������ͨ��5��ַ
	SPI_Write_Buf( NRF_WRITE_REG + TX_ADDR, nrf->TX_ADDR_, nrf->SETUP_AW_+2 );//����TX�˷��͵�ַ
	memset(rx_buf, 0, nrf->SETUP_AW_+2);
	SPI_Read_Buf( TX_ADDR, rx_buf, nrf->SETUP_AW_+2 );
	for ( i=0; i<nrf->SETUP_AW_+2; i++ ) {
		if ( rx_buf[i] != nrf->TX_ADDR_[i] ) {
			return pdFALSE;
		}
	}
	if ( !(nrf->DYNPD_&DPL_P0) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, nrf->RX_PW_P0_ );//��������ͨ��0��̬��Ч�غɳ���
	if ( !(nrf->DYNPD_&DPL_P1) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P1, nrf->RX_PW_P1_ );//��������ͨ��1��̬��Ч�غɳ���
	if ( !(nrf->DYNPD_&DPL_P2) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P2, nrf->RX_PW_P2_ );//��������ͨ��2��̬��Ч�غɳ���
	if ( !(nrf->DYNPD_&DPL_P3) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P3, nrf->RX_PW_P3_ );//��������ͨ��3��̬��Ч�غɳ���
	if ( !(nrf->DYNPD_&DPL_P4) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P4, nrf->RX_PW_P4_ );//��������ͨ��4��̬��Ч�غɳ���
	if ( !(nrf->DYNPD_&DPL_P5) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P5, nrf->RX_PW_P5_ );//��������ͨ��5��̬��Ч�غɳ���
	SPI_RW_Reg( NRF_WRITE_REG + DYNPD, nrf->DYNPD_ );//DYNPD ���ö�̬����(RX��)
	SPI_RW_Reg( NRF_WRITE_REG + FEATURE, nrf->FEATURE_ );//FEATURE ���ö�̬����(TX��)
	CE_High();
	return pdTRUE;
}

void Tx_Mode( void ) {
	uint8_t status;
	CSN_Low();
	status = SPI_RW_Reg( RD_RX_PLOAD + EN_AA, NOP ) & ( ~(PRIM_RX) );
	SPI_RW_Reg( NRF_WRITE_REG + EN_AA, status );
	CSN_High();
}

void Rx_Mode( void ) {
	uint8_t status;
	CSN_Low();
	status = SPI_RW_Reg( RD_RX_PLOAD + EN_AA, NOP ) | ( PRIM_RX );
	SPI_RW_Reg( NRF_WRITE_REG + EN_AA, status );
	CSN_High();
}



void nrf_init(void) {
	NRF24L01_TypeDef nrf;
	memset( &nrf, 0, sizeof(NRF24L01_TypeDef) );
	//nrf.CONFIG_ = EN_CRC|CRCO|PWR_UP|PRIM_RX;//RX
	nrf.CONFIG_ = EN_CRC|CRCO|PWR_UP;//TX
	nrf.EN_AA_ = ENAA_P0;//|ENAA_P1|ENAA_P2|ENAA_P3|ENAA_P4|ENAA_P5;
	nrf.EN_RXADDR_ = ERX_P0;//|ERX_P1|ERX_P2|ERX_P3|ERX_P4|ERX_P5;
	nrf.SETUP_AW_ = AW_WIDTH_4_BYTE;
	nrf.SETUP_RETR_ = AUTO_RETRANSMIT_DELAY_US(500) | AUTO_RETRANSMIT_COUNT(5);
	nrf.RF_CH_ = 2;//2402MHz
	nrf.RF_SETUP_ = RF_DR_1Mbps|RF_PWR_0dB;
	nrf.STATUS_ = RX_DR|TX_DS|MAX_RT;//��ձ�־
	memcpy( nrf.RX_ADDR_P0_, TX_ADDRESS, 5 );
	memcpy( nrf.TX_ADDR_, TX_ADDRESS, 5 );
	nrf.RX_PW_P0_ = 32;
	nrf.DYNPD_ = 0;
	nrf.FEATURE_ = 0;
	
	if ( Nrf24l01_Init( &nrf ) != pdTRUE ) {
		printf("***spi1 nrf24l01 ��ʼ��ʧ��!\r\n");
	} else {
		printf("nrf1 ��ʼ���ɹ�!\r\n");
	}
	
}

void nrf_receive_data(void) {
	uint8_t sta;//,temp;
	sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF��ָ��
	if ( sta & RX_DR ) {
		SPI_Read_Buf( RD_RX_PLOAD, rx_buf, RX_PLOAD_WIDTH );
		printf("%s\n",rx_buf);
		//SPI_RW_Reg(FLUSH_RX,NOP);
		SPI_RW_Reg( NRF_WRITE_REG + STATUS, sta );
	}
	//HAL_NVIC_EnableIRQ(EXTI17_IRQn);
}

void nrf_send_data( void ) {
	SPI_Write_Buf(WR_TX_PLOAD , tx_buf , 32 );
}

void receive( void ) {
	uint8_t sta;
	//if ( HAL_GPIO_ReadPin( NRF_IRQ_GPIO_Port, NRF_IRQ_Pin ) == GPIO_PIN_SET ) {
		sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF��ָ��
		if ( sta & RX_DR ) {
			SPI_Read_Buf( RD_RX_PLOAD, rx_buf, RX_PLOAD_WIDTH );
			printf("�յ�NRF����: %s\r\n",rx_buf);
			//SPI_RW_Reg(FLUSH_RX,NOP);
			SPI_RW_Reg( NRF_WRITE_REG + STATUS, sta );
		}
	//}
}
