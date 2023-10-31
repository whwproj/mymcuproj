#include "../Bsp/bsp_nrf.h"

NRF_STR nrf_str;

static int register_replay_device( uint8_t deviceId );
	
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

int Tx_Mode( uint8_t deviceId ) {
	uint8_t status;
	//根据deviceId获取nrfAddr
	if ( get_nrfaddr_by_deviceId( deviceId ) == 0 ) {
		return -1;//查找不到nrfAddr, 设备未注册
	}
	CE_Low();
	status = SPI_RW_Reg( NRF_READ_REG + CONFIG, NOP ) & ( ~(PRIM_RX) );
	SPI_RW_Reg( NRF_WRITE_REG + CONFIG, status );
	//更改地址为目标设备地址(增强模式收发地址必须一致)
	SPI_Write_Buf( NRF_WRITE_REG+TX_ADDR, nrf_str.deviceAddr, 4 );
	SPI_Write_Buf( NRF_WRITE_REG+RX_ADDR_P0, nrf_str.deviceAddr, 4 );
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, 0xF0 );
	SPI_RW_Reg( FLUSH_RX,NOP );
	SPI_RW_Reg( FLUSH_TX,NOP );
	CE_High();
	return 0;
}

void Rx_Mode( void ) {
	uint8_t status;
	CE_Low();
	status = SPI_RW_Reg( NRF_READ_REG + CONFIG, NOP ) | ( PRIM_RX );
	SPI_RW_Reg( NRF_WRITE_REG + CONFIG, status );
	//更改地址为本机地址(增强模式收发地址必须一致)
	SPI_Write_Buf( NRF_WRITE_REG+TX_ADDR, nrf_str.TSAddr, 4 );
	SPI_Write_Buf( NRF_WRITE_REG+RX_ADDR_P0, nrf_str.TSAddr, 4 );
	memset( nrf_str.txBuf, 0 , 33 );
	memset( nrf_str.rxBuf, 0 , 33 );
	CE_High();
}

void nrf_deInit(void) {
	CE_Low();
	if ( nrf_str.txBuf != NULL ) {
		vPortFree( nrf_str.txBuf );
		nrf_str.txBuf = NULL;
	}
	if ( nrf_str.rxBuf != NULL ) {
		vPortFree( nrf_str.rxBuf );
		nrf_str.rxBuf = NULL;
	}
}

//中转站默认地址: {0xC8,0x8F,0xE6,0x96}
void nrf_init(void) {
	NRF24L01_TypeDef nrf;
	memset( &nrf, 0, sizeof(NRF24L01_TypeDef) );
	
	//NRF默认地址
	memcpy( nrf.RX_ADDR_P0_, nrf_str.TSAddr, 4 );
	memcpy( nrf.TX_ADDR_, nrf_str.TSAddr, 4 );
	if ( nrf_str.txBuf == NULL ) nrf_str.txBuf = pvPortMalloc(33);
	if ( nrf_str.rxBuf == NULL ) nrf_str.rxBuf = pvPortMalloc(33);
	memset(nrf_str.txBuf, 0, 33);
	memset(nrf_str.rxBuf, 0, 33);
	
	nrf.CONFIG_ = EN_CRC|CRCO|PWR_UP|PRIM_RX;//RX
	nrf.EN_AA_ = ENAA_P0;//|ENAA_P1|ENAA_P2|ENAA_P3|ENAA_P4|ENAA_P5;
	nrf.EN_RXADDR_ = ERX_P0;//|ERX_P1|ERX_P2|ERX_P3|ERX_P4|ERX_P5;
	nrf.SETUP_AW_ = AW_WIDTH_4_BYTE;
	nrf.SETUP_RETR_ = AUTO_RETRANSMIT_DELAY_US(250) | AUTO_RETRANSMIT_COUNT(5);
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
		nrf_str.isInit = 1;
	}
}

//数据头:
//deviceId	1byte
//code			2byte
//msgType		1byte 0:注册设备 1:命令(转发->设备) 2:回复(设备->转发) 3:推送(设备->转发)
void nrf_receive_data(void) {
	uint8_t sta, deviceId, msgType;
	uint16_t code;
	
	sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF空指令
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, sta );
	printf("sta: 0x%.2X\r\n", sta);
	if ( sta & RX_DR ) {
		SPI_Read_Buf( RD_RX_PLOAD, nrf_str.rxBuf, RX_PLOAD_WIDTH );
		printf("nrfdata: ");
		for ( int i=0; i<10; i++ )
			printf(" 0x%.2X", nrf_str.rxBuf[i]);
		printf("\r\n");
		//LED_NRF_ON();
	} else {
		goto end;
	}
	
	nrf_str.statusRegSetNum = 0;//重置定时查询寄存器计数器
	deviceId = nrf_str.rxBuf[0];
	msgType = nrf_str.rxBuf[3];//0:注册设备 1:中转站->设备 2:(回复)设备->中转站 3:(推送)设备->中转站 4:心跳
	code = nrf_str.rxBuf[1]<<8 | nrf_str.rxBuf[2];
	if ( msgType == 0 ) {
		//设备绑定:根据deviceId查询nrfAddr是否一致,一致跳过,不一致或不存在则更新或添加
		if ( deviceId == 0 ) {
			goto end;
		} else if ( deviceId == 0xFF ) {
			create_deviceId( deviceId );
		} else if ( !get_nrfaddr_by_deviceId(deviceId) ||
				nrf_str.rxBuf[4]!=nrf_str.deviceAddr[0] || nrf_str.rxBuf[5]!=nrf_str.deviceAddr[1] ||
				nrf_str.rxBuf[6]!=nrf_str.deviceAddr[2] || nrf_str.rxBuf[7]!=nrf_str.deviceAddr[3] ) {
			deviceId = insert_nrfaddr( deviceId );
		}
		//回复设备
		if ( register_replay_device( deviceId ) != -1 ) {
			//推送MQTT,设备上线
			wifi_msg_add_SendList( deviceId, code, "Device online!", 0 );
		}
		
	} else if ( msgType == 1 ) {//中转站->设备
	} else if ( msgType == 2 ) {//(回复)设备->中转站 
	} else if ( msgType == 3 ) {//(推送)设备->中转站
		wifi_msg_add_SendList( deviceId, code, (char*)&nrf_str.rxBuf[4], 0 );
	} else if ( msgType == 4 ) {//心跳
		nrf_str.onlineDeviceList[deviceId] = 10;
	}
	
	end:
	//LED_NRF_OFF();
	Rx_Mode();
	__HAL_GPIO_EXTI_CLEAR_IT(NRF_IRQ_Pin);
	HAL_NVIC_EnableIRQ(NRF_IRQ_EXTI_IRQn);
	nrf_str.session.cacheLock = 0;//解锁缓存
}

//包装nrf要发送数据
//msgType: 
//		0:注册设备 设备->中转站
//		1:命令 中转站->设备
//		2:回复 设备 -> 中转站
//		3:推送 设备 -> 中转站
void nrf_pack_data( uint8_t did, uint16_t code, char* pdata ) {
	memset( nrf_str.txBuf, 0, 33 );
	nrf_str.txBuf[0] = did;
	nrf_str.txBuf[1] = code >> 8;
	nrf_str.txBuf[2] = code;
	nrf_str.txBuf[3] = 1;//msgType
	if ( strlen( pdata ) <= 28 ) {
		sprintf( (char*)&nrf_str.txBuf[4], "%s", pdata );
	} else {
		sprintf( (char*)&nrf_str.txBuf[4], "data too long..." );
	}
	vPortFree( data_str.data );
	data_str.data = NULL;
}

// //NRF发送数据
// int nrf_send_data( uint8_t deviceId ) {
// 	int sta = 0;

// 	HAL_NVIC_DisableIRQ( NRF_IRQ_EXTI_IRQn );
// 	if ( Tx_Mode( deviceId ) != 0 ) {
// 		//发送设备未注册事件
// 		w_str.session.deviceId = nrf_str.txBuf[0];
// 		w_str.session.code = (nrf_str.txBuf[1]<<8) | nrf_str.txBuf[2];
// 		//xTaskNotify( wifi_control_taskHandle, 1U<<DEVICE_NOT_REGISTER, eSetBits );
// 		sta = -1;
// 		return;
// 	}
// 	SPI_Write_Buf( WR_TX_PLOAD , nrf_str.txBuf , 32 );
// 	do {
// 		vTaskDelay(2);
// 		sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );
// 	} while ( (sta & ((TX_DS)|(MAX_RT))) == 0 );
// 	if ( sta & (TX_DS) ) {//发送成功
// 		sta = 0;
// 	} else if ( sta & (MAX_RT) ) {//重发次数达到最大,发送失败
// 		//发送设备离线事件 
// 		w_str.session.deviceId = nrf_str.txBuf[0];
// 		w_str.session.code = (nrf_str.txBuf[1]<<8)|nrf_str.txBuf[2];
// 		//xTaskNotify( wifi_control_taskHandle, 1U<<DEVICE_NOT_ONLINE, eSetBits );
// 		sta = -1;
// 	}
// 	SPI_RW_Reg( NRF_WRITE_REG + STATUS, 0xF0 );
// 	SPI_RW_Reg( FLUSH_TX, NOP );
	
// 	Rx_Mode();
// 	return sta;
// }

int nrf_send_data( void ) {
	int sta = 0;
	if ( Tx_Mode( nrf_str.session.deviceId ) != 0 ) {
		//发送设备未注册事件
		wifi_msg_add_SendList( nrf_str.session.deviceId, nrf_str.session.code, "Device not register!", -2 );
		sta = -1;
		goto end;
	}
	SPI_Write_Buf( WR_TX_PLOAD , nrf_str.txBuf , 32 );
	do {
		vTaskDelay(2);
		sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );
	} while ( (sta & ((TX_DS)|(MAX_RT))) == 0 );
	if ( sta & (TX_DS) ) {//发送成功
		printf("转发成功\r\n");
		sta = 0;
		nrf_str.statusRegSetNum = 0;
	} else if ( sta & (MAX_RT) ) {//重发次数达到最大,发送失败
		printf("转发失败\r\n");
		//发送设备离线事件
		wifi_msg_add_SendList( nrf_str.session.deviceId, nrf_str.session.code, "Device not online!", -1 );
		sta = -1;
	}
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, 0xF0 );
	SPI_RW_Reg( FLUSH_TX, NOP );
	end:
	return sta;
}


static int register_replay_device( uint8_t deviceId ) {
	//封装数据
	nrf_str.txBuf[0] = deviceId;
	nrf_str.txBuf[1] = nrf_str.rxBuf[1];
	nrf_str.txBuf[2] = nrf_str.rxBuf[2];
	nrf_str.txBuf[3] = 0;
	nrf_str.session.deviceId = deviceId;
	memcpy( &nrf_str.txBuf[4], &nrf_str.rxBuf[4], 4 );
	//发送数据
	return nrf_send_data();
}

//定时检查寄存器状态并清除
void tim_clear_nrfreg_isr( void ) {
	BaseType_t phpt;
	if ( nrf_str.isInit ) {
		if ( nrf_str.statusRegSetNum++ > 12 ) {
			nrf_str.statusRegSetNum = 0;
			xTaskNotifyFromISR( nrf_control_taskHandle, 1U<<TIM_CLEAR_NRFREG, eSetBits, &phpt );
			portYIELD_FROM_ISR( phpt );
		}
	}
}

//定时检查寄存器状态并清除
void tim_clear_nrfreg( void ) {
	//BaseType_t phpt;
	uint8_t sta;
	sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF空指令
	if ( sta & ( (RX_DR)|(TX_DS)|(MAX_RT)|(TX_FULL) ) ) {
		//ul = taskENTER_CRITICAL_FROM_ISR();
		printf( "寄存器状态异常 sta: 0x%.2X\r\n", sta );
		SPI_RW_Reg( NRF_WRITE_REG + STATUS, 0xF0 );
		SPI_RW_Reg( FLUSH_RX, NOP );
		SPI_RW_Reg( FLUSH_TX, NOP );
		Rx_Mode();
		__HAL_GPIO_EXTI_CLEAR_IT(NRF_IRQ_Pin);
		HAL_NVIC_EnableIRQ(NRF_IRQ_EXTI_IRQn);
		sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF空指令
		printf( "清除异常状态寄存器 sta: 0x%.2X\r\n", sta );
		//taskEXIT_CRITICAL_FROM_ISR(ul);
	}
}







