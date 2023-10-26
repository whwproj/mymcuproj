#include "../Bsp/bsp_nrf.h"

NRF_STR nrf_str;

static void register_replay_device( uint8_t deviceId );
	
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
	//更改发送地址为目标设备的接收地址
	SPI_Write_Buf( NRF_WRITE_REG+TX_ADDR, nrf_str.rxAddr, 4 );
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
	//更改发送地址为本机默认发送地址
	SPI_Write_Buf( NRF_WRITE_REG+TX_ADDR, nrf_str.TS_txAddr, 4 );
	memset( nrf_str.txBuf, 0 , 33 );
	memset( nrf_str.rxBuf, 0 , 33 );
	CE_High();
}

//中转站地址 默认接收地址: 0x1A,0x2B,0x3C,0x4D
//中转站地址 默认发送地址: 0xA1,0xB2,0xC3,0xD4
void nrf_init(void) {
	NRF24L01_TypeDef nrf;
	memset( &nrf, 0, sizeof(NRF24L01_TypeDef) );
	
	//NRF默认地址
	memcpy( nrf.RX_ADDR_P0_, nrf_str.TS_rxAddr, 4 );
	memcpy( nrf.TX_ADDR_, nrf_str.TS_txAddr, 4 );
	nrf_str.txBuf = pvPortMalloc(33);
	nrf_str.rxBuf = pvPortMalloc(33);
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
	}
}

//数据头:
//deviceId	1byte
//code			2byte
//msgType		1byte 0:注册设备 1:命令(转发->设备) 2:回复(设备->转发) 3:推送(设备->转发)
void nrf_receive_data(void) {
	//char *str;
	uint8_t sta;
	
	sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF空指令
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, sta );
	printf("sta: 0x%.2X\r\n", sta);
	if ( sta & RX_DR ) {
		SPI_Read_Buf( RD_RX_PLOAD, nrf_str.rxBuf, RX_PLOAD_WIDTH );
		//printf("%s\n",nrf_str.rxBuf);
		printf("nrfdata: \r\n");
		for ( int i=0; i<10; i++ )
			printf(" 0x%.2X", nrf_str.rxBuf[i]);
		printf("\r\n");goto end;
	} else {
		goto end;
	}

	if ( nrf_str.rxBuf[3] == 0 ) {
		//设备绑定:根据deviceId查询nrfAddr是否一致,一致跳过,不一致或不存在则更新或添加
		if ( nrf_str.rxBuf[0] == 0xFF ) {
			create_deviceId( nrf_str.rxBuf[0] );
		} else if ( !get_nrfaddr_by_deviceId(nrf_str.rxBuf[0]) ||
				nrf_str.rxBuf[4]!=nrf_str.rxAddr[0] || nrf_str.rxBuf[5]!=nrf_str.rxAddr[1] ||
				nrf_str.rxBuf[6]!=nrf_str.rxAddr[2] || nrf_str.rxBuf[7]!=nrf_str.rxAddr[3] ) {
			nrf_str.rxBuf[0] = insert_nrfaddr( nrf_str.rxBuf[0] );
		}
		//回复设备
		register_replay_device( nrf_str.rxBuf[0] );
		//推送MQTT
		//xTaskNotify()
		return;
		
	} else if ( nrf_str.rxBuf[3] == 1 ) {
		return;
	
	} else if ( nrf_str.rxBuf[3] == 2 ) {//设备回复:携带pcode转发mqtt
		//删除超时未应答链表节点
		while( ask_str.useing ) { osDelay(1); }
		ask_str.useing = 1;
		for ( int i=0; i<5; i++ ) {
			if ( ask_str.deviceIds[i] == nrf_str.rxBuf[0] ) {
				ask_str.list &=~ (1<<i);
				ask_str.deviceIds[i] = 0;
				ask_str.sendTicks[i] = 0;
				ask_str.pcode[i] = 0;
			}
		}
		ask_str.useing = 0;
		while ( !w_str.sendLock ) { osDelay(1); }
		w_str.session.deviceId = nrf_str.rxBuf[0];
		w_str.session.code = (nrf_str.rxBuf[1]<<8) | nrf_str.rxBuf[2];
		send_forward_success();
		
	} else if ( nrf_str.rxBuf[3] == 3 ) {//设备推送:转发mqtt
		push_data_fun((char*)&nrf_str.rxBuf[4]);
	}
	
	end:
	printf("开启中断\r\n");
	memset( nrf_str.rxAddr, 0 , 33 );
	__HAL_GPIO_EXTI_CLEAR_IT(NRF_IRQ_Pin);
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
	nrf_str.txBuf[3] = 1;//msgType
	if ( strlen( pdata ) <= 28 ) {
		sprintf( (char*)&nrf_str.txBuf[4], "%s", pdata );
	} else {
		sprintf( (char*)&nrf_str.txBuf[4], "data too long..." );
	}
	vPortFree( data_str.data );
}

//NRF发送数据
int nrf_send_data( uint8_t deviceId ) {
	uint8_t sta = 0;

	HAL_NVIC_DisableIRQ( NRF_IRQ_EXTI_IRQn );
	if ( Tx_Mode( deviceId ) != 0 ) {
		//发送设备未注册事件
		w_str.session.deviceId = nrf_str.txBuf[0];
		w_str.session.code = (nrf_str.txBuf[1]<<8) | nrf_str.txBuf[2];
		xTaskNotify( wifi_control_taskHandle, 1U<<DEVICE_NOT_REGISTER, eSetBits );
		return -1;
	}
	SPI_Write_Buf( WR_TX_PLOAD , nrf_str.txBuf , 32 );
	do {
		vTaskDelay(2);
		sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );
	} while ( (sta & ((TX_DS)|(MAX_RT))) == 0 );
	if ( sta & (TX_DS) ) {//发送成功
		sta = 0;
	} else if ( sta & (MAX_RT) ) {//重发次数达到最大,发送失败
		//发送设备离线事件 
		w_str.session.deviceId = nrf_str.txBuf[0];
		w_str.session.code = (nrf_str.txBuf[1]<<8)|nrf_str.txBuf[2];
		xTaskNotify( wifi_control_taskHandle, 1U<<DEVICE_NOT_ONLINE, eSetBits );
		sta = -1;
	}
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, 0xF0 );
	SPI_RW_Reg( FLUSH_TX,NOP );
	
	Rx_Mode();
	__HAL_GPIO_EXTI_CLEAR_IT(NRF_IRQ_Pin);
	HAL_NVIC_EnableIRQ(NRF_IRQ_EXTI_IRQn);		
	return sta;
}


static void register_replay_device( uint8_t deviceId ) {
	//封装数据
	nrf_str.txBuf[0] = deviceId;
	nrf_str.txBuf[1] = nrf_str.rxBuf[1];
	nrf_str.txBuf[2] = nrf_str.rxBuf[2];
	nrf_str.txBuf[3] = 0;
	memcpy( &nrf_str.txBuf[4], &nrf_str.rxBuf[4], 4 );
	//发送数据
	nrf_send_data( deviceId );
}

void nrf_send_test( void ) {
	uint8_t status, sta;
	
	HAL_NVIC_DisableIRQ( NRF_IRQ_EXTI_IRQn );
	sprintf((char*)nrf_str.txBuf, "abcd1234567");
	
	CE_Low();osDelay(1);
	CSN_Low();
	status = SPI_RW_Reg( NRF_READ_REG + CONFIG, NOP ) & ( ~(PRIM_RX) );
	SPI_RW_Reg( NRF_WRITE_REG + CONFIG, status );
	CSN_High();
	CE_High();osDelay(1);
	
	SPI_Write_Buf( WR_TX_PLOAD , nrf_str.txBuf , 32 );
	vTaskDelay(10);
	
	sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );
	printf("sta: 0x%.2X\r\n", sta);
	if ( sta & MAX_RT ) {
		printf("通信失败\r\n");
		SPI_RW_Reg( NRF_WRITE_REG + STATUS, 0xF0 );
		SPI_RW_Reg( FLUSH_TX,NOP );
	} else {
		printf("发送成功!\r\n");
	}
	
	
	//SPI_RW_Reg( FLUSH_RX,NOP );
	
	
}


