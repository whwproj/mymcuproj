#include "../Bsp/bsp_nrf.h"

NRF_STR nrf_str;

uint8_t SPI_RW_Reg( uint8_t reg, uint8_t value ) {//读写寄存器
	uint8_t status; 
	CSN_Low();
	HAL_SPI_Transmit( &NRF_SPI_Handle, &reg, 1, 100 );
	HAL_SPI_TransmitReceive( &NRF_SPI_Handle, &value, &status, 1, 100 );
	CSN_High();
	return(status);
} 

uint8_t SPI_Write_Buf( uint8_t reg, uint8_t *pBuf, uint8_t len ) {//多字节写入
	uint8_t status; 
	CSN_Low();
	HAL_SPI_TransmitReceive( &NRF_SPI_Handle, &reg, &status, 1, 100 );
	HAL_SPI_Transmit( &NRF_SPI_Handle, pBuf, len, 100 );
	CSN_High();
	return(status);  
}  

uint8_t SPI_Read_Buf( uint8_t reg, uint8_t *pBuf, uint8_t len ) {//多字节读取
	uint8_t status; 
	CSN_Low();
	HAL_SPI_TransmitReceive( &NRF_SPI_Handle, &reg, &status, 1, 100 );
	HAL_SPI_Receive( &NRF_SPI_Handle, pBuf, len, 100);
	CSN_High();
	return(status); // return nRF24L01 status uchar 
}


uint8_t Nrf24l01_Init( NRF24L01_TypeDef *nrf ) {
	uint8_t i;
	CE_High();
	osDelay(100);
	CE_Low();
	osDelay(100);
	SPI_RW_Reg( FLUSH_TX, NOP );//清空FIFO
	SPI_RW_Reg( FLUSH_RX, NOP );//清空FIFO
	SPI_RW_Reg( NRF_WRITE_REG + CONFIG, nrf->CONFIG_ );//CONFIG 工作模式配置寄存器
	i = SPI_RW_Reg( NRF_READ_REG+CONFIG, NOP );
	SPI_RW_Reg( NRF_WRITE_REG + EN_AA, nrf->EN_AA_ );//EN_AA 使能自动确认功能(Enhanced ShockBurst)
	SPI_RW_Reg( NRF_WRITE_REG + EN_RXADDR, nrf->EN_RXADDR_ );//EN_RXADDR 使能接收通道(0~5)
	SPI_RW_Reg( NRF_WRITE_REG + SETUP_AW, nrf->SETUP_AW_ );//SETUP_AW 设置地址宽度[3-5]byte
	SPI_RW_Reg( NRF_WRITE_REG + SETUP_RETR, nrf->SETUP_RETR_ );//SETUP_RETR 设置自动重传
	SPI_RW_Reg( NRF_WRITE_REG + RF_CH, nrf->RF_CH_ );//RF_CH 射频频率
	SPI_RW_Reg( NRF_WRITE_REG + RF_SETUP, nrf->RF_SETUP_ );//RF_SETUP 射频设置寄存器
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, nrf->STATUS_ );//STATUS 中断状态寄存器
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P0, nrf->RX_ADDR_P0_, nrf->SETUP_AW_+2 );//配置数据通道0地址
	//SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P1, nrf->RX_ADDR_P1_, nrf->SETUP_AW_+2 );//配置数据通道1地址
	//SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P2, nrf->RX_ADDR_P2_, nrf->SETUP_AW_+2 );//配置数据通道2地址
	//SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P3, nrf->RX_ADDR_P3_, nrf->SETUP_AW_+2 );//配置数据通道3地址
	//SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P4, nrf->RX_ADDR_P4_, nrf->SETUP_AW_+2 );//配置数据通道4地址
	//SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P5, nrf->RX_ADDR_P5_, nrf->SETUP_AW_+2 );//配置数据通道5地址
	SPI_Write_Buf( NRF_WRITE_REG + TX_ADDR, nrf->TX_ADDR_, nrf->SETUP_AW_+2 );//配置TX端发送地址
	memset(nrf_str.rxBuf, 0, nrf->SETUP_AW_+2);
	SPI_Read_Buf( TX_ADDR, nrf_str.rxBuf, nrf->SETUP_AW_+2 );
	for ( i=0; i<nrf->SETUP_AW_+2; i++ ) {
		if ( nrf_str.rxBuf[i] != nrf->TX_ADDR_[i] ) {
			return pdFALSE;
		}
	}
	if ( !(nrf->DYNPD_&DPL_P0) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, nrf->RX_PW_P0_ );//设置数据通道0静态有效载荷长度
	if ( !(nrf->DYNPD_&DPL_P1) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P1, nrf->RX_PW_P1_ );//设置数据通道1静态有效载荷长度
	if ( !(nrf->DYNPD_&DPL_P2) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P2, nrf->RX_PW_P2_ );//设置数据通道2静态有效载荷长度
	if ( !(nrf->DYNPD_&DPL_P3) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P3, nrf->RX_PW_P3_ );//设置数据通道3静态有效载荷长度
	if ( !(nrf->DYNPD_&DPL_P4) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P4, nrf->RX_PW_P4_ );//设置数据通道4静态有效载荷长度
	if ( !(nrf->DYNPD_&DPL_P5) ) SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P5, nrf->RX_PW_P5_ );//设置数据通道5静态有效载荷长度
	SPI_RW_Reg( NRF_WRITE_REG + DYNPD, nrf->DYNPD_ );//DYNPD 配置动态长度(RX端)
	SPI_RW_Reg( NRF_WRITE_REG + FEATURE, nrf->FEATURE_ );//FEATURE 配置动态长度(TX端)
	CE_High();
	return pdTRUE;
}

int Tx_Mode( void ) {
	uint8_t status;
	CE_Low();osDelay(1);
	//根据deviceId获取nrfAddr
	if ( get_nrfaddr_by_deviceId( nrf_str.txBuf[0] ) == 0 ) {
		//查找不到nrfAddr, 设备未注册
		return -1;
	}
	//配置TX端发送地址
	SPI_Write_Buf( NRF_WRITE_REG + TX_ADDR, nrf_str.txAddr, 4 );
	CSN_Low();
	status = SPI_RW_Reg( RD_RX_PLOAD + EN_AA, NOP ) & ( ~(PRIM_RX) );
	SPI_RW_Reg( NRF_WRITE_REG + EN_AA, status );
	CSN_High();
	CE_High();osDelay(1);
	
	return 0;
}

void Rx_Mode( void ) {
	uint8_t status;
	CE_Low();osDelay(1);
	CSN_Low();
	status = SPI_RW_Reg( RD_RX_PLOAD + EN_AA, NOP ) | ( PRIM_RX );
	SPI_RW_Reg( NRF_WRITE_REG + EN_AA, status );
	CSN_High();
	CE_High();osDelay(1);
}

void nrf_init(void) {
	NRF24L01_TypeDef nrf;
	uint8_t txaddr_t[4] = {0x0A,0x0B,0x0C,0x0D};
	memset( &nrf, 0, sizeof(NRF24L01_TypeDef) );
	memcpy( nrf.RX_ADDR_P0_, udata.snId, 4 );
	memcpy( nrf.TX_ADDR_, txaddr_t, 4 );
	
	nrf.CONFIG_ = MASK_RX_DR|MASK_MAX_RT|EN_CRC|CRCO|PWR_UP|PRIM_RX;//RX
	nrf.EN_AA_ = ENAA_P0;//|ENAA_P1|ENAA_P2|ENAA_P3|ENAA_P4|ENAA_P5;
	nrf.EN_RXADDR_ = ERX_P0;//|ERX_P1|ERX_P2|ERX_P3|ERX_P4|ERX_P5;
	nrf.SETUP_AW_ = AW_WIDTH_4_BYTE;
	nrf.SETUP_RETR_ = AUTO_RETRANSMIT_DELAY_US(500) | AUTO_RETRANSMIT_COUNT(5);
	nrf.RF_CH_ = 2;//2402MHz
	nrf.RF_SETUP_ = RF_DR_1Mbps|RF_PWR_0dB;
	nrf.STATUS_ = RX_DR|TX_DS|MAX_RT;//清空标志
	nrf.RX_PW_P0_ = 32;
	nrf.DYNPD_ = 0;
	nrf.FEATURE_ = 0;
	
	if ( Nrf24l01_Init( &nrf ) != pdTRUE ) {
		printf("***spi1 nrf24l01 初始化失败!\r\n");
	} else {
		printf("nrf1 初始化成功!\r\n");
	}
}

//数据头:
//deviceId	1byte
//code			2byte
//msgType		1byte 0:注册设备 1:命令(转发->设备) 2:回复(设备->转发) 3:推送(设备->转发)
void nrf_receive_data(void) {
	//char *str;
	uint8_t sta;//, temp;
	
	sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF空指令
	if ( sta & RX_DR ) {
		SPI_Read_Buf( RD_RX_PLOAD, nrf_str.rxBuf, RX_PLOAD_WIDTH );
		//printf("%s\n",nrf_str.rxBuf);
		switch( nrf_str.rxBuf[3] ) {//msgType
			case 0://设备绑定:根据deviceId查询nrfAddr是否一致,一致跳过,不一致或不存在则更新或添加
				get_nrfaddr_by_deviceId( nrf_str.rxBuf[0] );
				if ( nrf_str.rxBuf[4]!=nrf_str.txAddr[0] || nrf_str.rxBuf[5]!=nrf_str.txAddr[1] ||
						 nrf_str.rxBuf[6]!=nrf_str.txAddr[2] || nrf_str.rxBuf[7]!=nrf_str.txAddr[3] ) {
					insert_nrfaddr( nrf_str.rxBuf[0] );
				}
				break;
			
			case 1: break;
			
			case 2://设备回复:携带pcode转发mqtt
				while ( !w_str.sendLock ) { osDelay(1); }
				w_str.session.deviceId = nrf_str.rxBuf[0];
				w_str.session.code = (nrf_str.txAddr[1]<<8) | nrf_str.txAddr[2];
				send_forward_success();
				break;
			
			case 3://设备推送:转发mqtt
				push_data_fun((char*)&nrf_str.rxBuf[4]);
				break;
		}
	}
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, sta );
	HAL_NVIC_EnableIRQ(NRF_IRQ_EXTI_IRQn);
}

//包装nrf要发送数据
//msgType: 
//		0:注册设备 设备->中转站
//		1:命令 中转站->设备
//		2:回复 设备 -> 中转站
//		3:推送 设备 -> 中转站
void nrf_pack_data( uint8_t did, uint16_t code, char* pdata ) {
	while( nrf_str.notEmpty ) { vTaskDelay(10); }
	memset( nrf_str.txBuf, 0, 33 );
	nrf_str.notEmpty = 1;
	nrf_str.txBuf[0] = did;
	nrf_str.txBuf[1] = code >> 8;
	nrf_str.txBuf[2] = code;
	nrf_str.txBuf[3] = 1;
	if ( strlen( pdata ) <= 28 ) {
		sprintf( (char*)&nrf_str.txBuf[4], "%s", pdata );
	} else {
		sprintf( (char*)&nrf_str.txBuf[4], "data too long..." );
	}
}

//NRF发送数据
void nrf_send_data( void ) {
	uint8_t sta;
	
	HAL_NVIC_DisableIRQ( NRF_IRQ_EXTI_IRQn );
	
	for ( int i,n=0; ; n++ ) {
		if ( Tx_Mode() != 0 ) {
			//发送设备未注册事件
			w_str.session.deviceId = nrf_str.txBuf[0];
			w_str.session.code = (nrf_str.txBuf[1]<<8) | nrf_str.txBuf[2];
			xTaskNotify( wifi_control_taskHandle, 1U<<DEVICE_NOT_REGISTER, eSetBits );
			break;
		}
		SPI_Write_Buf( WR_TX_PLOAD , nrf_str.txBuf , 32 );
		for ( i=0; i<0xFF; i++ ) {
			sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );
			if ( !(sta&0x01) ) { /*printf( "数据通道为空,i计数次数:%d\r\n", i );*/break; }
		}
		taskENTER_CRITICAL();
		SPI_RW_Reg( NRF_WRITE_REG + STATUS, 0xF0 );
		SPI_RW_Reg( FLUSH_RX,NOP );
		SPI_RW_Reg( FLUSH_TX,NOP );
		taskEXIT_CRITICAL();
		if ( i == 0xFF ) {//发送失败
			if ( n >= 5 ) {//通信失败
				//发送设备离线事件 
				//nrf_str.notEmpty = 0;
				w_str.session.deviceId = nrf_str.txBuf[0];
				w_str.session.code = (nrf_str.txBuf[1]<<8)|nrf_str.txBuf[2];
				xTaskNotify( wifi_control_taskHandle, 1U<<DEVICE_NOT_ONLINE, eSetBits );
				break;
			}
			vTaskDelay(100); 
			
		} else {//发送成功,转Rx
			//发送转发成功事件
			//计时等待10ms应答
			add_data_to_list_fun( nrf_str.txBuf[0], (nrf_str.txBuf[1]<<8)|nrf_str.txBuf[2] );
			xTaskNotify( time_task_handle, 1U<<NRF_SEND_TIME, eSetBits );
			break;
		}
	}
	nrf_str.notEmpty = 0;//解锁
	Rx_Mode();
	__HAL_GPIO_EXTI_CLEAR_IT(NRF_IRQ_Pin);
	HAL_NVIC_EnableIRQ(NRF_IRQ_EXTI_IRQn);	
}

