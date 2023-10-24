#include "../Bsp/bsp_nrf.h"

NRF_STR nrf_str;

static uint16_t craeteGrowthCode( void );
static void nrf_replay_cmd( char* replay );
static void wait_tx_buff_empty( void );
	
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
	i = SPI_RW_Reg( NRF_READ_REG + CONFIG, NOP );
	printf( "CONFIG: %d\r\n", i );
	SPI_RW_Reg( NRF_WRITE_REG + EN_AA, nrf->EN_AA_ );//EN_AA 使能自动确认功能(Enhanced ShockBurst)
	i = SPI_RW_Reg( NRF_READ_REG + EN_AA, nrf->EN_AA_ );
	printf( "EN_AA: %d\r\n", i );
	SPI_RW_Reg( NRF_WRITE_REG + EN_RXADDR, nrf->EN_RXADDR_ );//EN_RXADDR 使能接收通道(0~5)
	SPI_RW_Reg( NRF_WRITE_REG + SETUP_AW, nrf->SETUP_AW_ );//SETUP_AW 设置地址宽度[3-5]byte
	SPI_RW_Reg( NRF_WRITE_REG + SETUP_RETR, nrf->SETUP_RETR_ );//SETUP_RETR 设置自动重传
	SPI_RW_Reg( NRF_WRITE_REG + RF_CH, nrf->RF_CH_ );//RF_CH 射频频率
	SPI_RW_Reg( NRF_WRITE_REG + RF_SETUP, nrf->RF_SETUP_ );//RF_SETUP 射频设置寄存器
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, nrf->STATUS_ );//STATUS 中断状态寄存器
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P0, nrf->RX_ADDR_P0_, nrf->SETUP_AW_+2 );//配置数据通道0地址
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P1, nrf->RX_ADDR_P1_, nrf->SETUP_AW_+2 );//配置数据通道1地址
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P2, nrf->RX_ADDR_P2_, nrf->SETUP_AW_+2 );//配置数据通道2地址
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P3, nrf->RX_ADDR_P3_, nrf->SETUP_AW_+2 );//配置数据通道3地址
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P4, nrf->RX_ADDR_P4_, nrf->SETUP_AW_+2 );//配置数据通道4地址
	SPI_Write_Buf( NRF_WRITE_REG + RX_ADDR_P5, nrf->RX_ADDR_P5_, nrf->SETUP_AW_+2 );//配置数据通道5地址
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

void Tx_Mode( void ) {
	uint8_t status;
	CE_Low();osDelay(1);
	CSN_Low();
	status = SPI_RW_Reg( NRF_READ_REG + CONFIG, NOP ) & ( ~(PRIM_RX) );
	SPI_RW_Reg( NRF_WRITE_REG + CONFIG, status );
	CSN_High();
	CE_High();osDelay(1);
}

void Rx_Mode( void ) {
	uint8_t status;
	CE_Low();osDelay(1);
	CSN_Low();
	status = SPI_RW_Reg( NRF_READ_REG + CONFIG, NOP ) | ( PRIM_RX );
	SPI_RW_Reg( NRF_WRITE_REG + CONFIG, status );
	CSN_High();
	CE_High();osDelay(1);
}


//中转站地址 0x1A,0x2B,0x3C,0x4D,0x5E
void nrf_init(void) {
	NRF24L01_TypeDef nrf;
	//uint8_t tx_t[5] = {0x1A,0x2B,0x3C,0x4D,0x5E};
	//uint8_t rx_t[5] = {0x0A,0x0B,0x0C,0x0D,0x11};
	uint8_t tx_t[5] = {0xA3,0xA3,0xA3,0xA3,0xA3};
	uint8_t rx_t[5] = {0xA3,0xA3,0xA3,0xA3,0xA3};
	//初始化结构体
	memset( &nrf, 0, sizeof(NRF24L01_TypeDef) );
	
	memcpy( nrf_str.rxAddr, rx_t, 5 );
	memcpy( nrf_str.txAddr, tx_t, 5 );
	nrf_str.txBuf = pvPortMalloc(33);
	nrf_str.rxBuf = pvPortMalloc(33);
	memset(nrf_str.txBuf, 0, 33);
	memset(nrf_str.rxBuf, 0, 33);
	
	nrf.CONFIG_ = EN_CRC|CRCO|PWR_UP|PRIM_RX;//RX
	//nrf.CONFIG_ = MASK_MAX_RT|MASK_TX_DS|EN_CRC|CRCO|PWR_UP;//TX
	nrf.EN_AA_ = ENAA_P0;//|ENAA_P1|ENAA_P2|ENAA_P3|ENAA_P4|ENAA_P5;
	nrf.EN_RXADDR_ = ERX_P0;//|ERX_P1|ERX_P2|ERX_P3|ERX_P4|ERX_P5;
	nrf.SETUP_AW_ = AW_WIDTH_4_BYTE;
	nrf.SETUP_RETR_ = AUTO_RETRANSMIT_DELAY_US(500) | AUTO_RETRANSMIT_COUNT(5);
	nrf.RF_CH_ = 2;//2402MHz
	nrf.RF_SETUP_ = RF_DR_1Mbps|RF_PWR_0dB;
	nrf.STATUS_ = RX_DR|TX_DS|MAX_RT;//清空标志
	memcpy( nrf.RX_ADDR_P0_, nrf_str.rxAddr, 5 );
	memcpy( nrf.TX_ADDR_, nrf_str.txAddr, 5 );
	nrf.RX_PW_P0_ = 32;
	nrf.DYNPD_ = 0;
	nrf.FEATURE_ = 0;
	
	if ( Nrf24l01_Init( &nrf ) != pdTRUE ) {
		printf("*** nrf24l01 初始化失败!\r\n");
	} else {
		printf("nrf 初始化成功!\r\n");
	}
}

//发送数据
int nrf_send_data( void ) {
	int res = 0;
	uint8_t sta;
	
	HAL_NVIC_DisableIRQ( NRF_IRQ_EXTI_IRQn );
	
	for ( int i,n=0; ; n++ ) {
		Tx_Mode();
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
			if ( nrf_str.sessionSta == 2 ) {//注册状态跳过,一直发送
			} else {
				if ( n >= 5 ) {//通信失败
					res = -1;
					if ( str.regSta == 2 ) {//发送数据失败
						str.regSta = 1;
						xTaskNotify( nrf_control_taskHandle, 1U<<NRF_REGISTER_DEVICE, eSetBits );
					}
					break;
				}
			}
			vTaskDelay(50); 
			
		} else {//发送成功,转Rx
			nrf_str.heartTime = 0;
			break;
		}
	}
	
	nrf_str.txBuffEmpty = 0;//标记发送空闲
	nrf_str.sessionSta = 0;//标记默认会话状态
	Rx_Mode();
	__HAL_GPIO_EXTI_CLEAR_IT(NRF_IRQ_Pin);
	HAL_NVIC_EnableIRQ(NRF_IRQ_EXTI_IRQn);	

	return res;
}

//解析数据
void nrf_parse_data( void ) {
	char *str;
	uint8_t sta;
	
	sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF空指令
	if ( sta & RX_DR ) {
		SPI_Read_Buf( RD_RX_PLOAD, nrf_str.rxBuf, RX_PLOAD_WIDTH );
		printf("%s\n",nrf_str.rxBuf);
	}
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, sta );
	HAL_NVIC_EnableIRQ(NRF_IRQ_EXTI_IRQn);
	
	if ( nrf_str.rxAddr[0] != DEVICE_ID ) {
		return;
	}
	nrf_str.code = nrf_str.rxAddr[1]<<8 | nrf_str.rxAddr[2];
	if ( nrf_str.rxAddr[3] != 0x01 ) {//msgType: 1:命令 中转站->设备
		return;
	}
	str = (char*)&nrf_str.rxAddr[4];
	if ( strstr( str, "LED_MODE_0" ) != NULL ) {
		nrf_replay_cmd("success!");
		xTaskNotify( executive_taskHandle, 1U<<LED_MODE_0, eSetBits );
		
	} else if ( strstr( str, "LED_MODE_OFF" ) != NULL ) {
		nrf_replay_cmd("success!");
		xTaskNotify( executive_taskHandle, 1U<<LED_MODE_OFF, eSetBits );
		
	}
	memset( nrf_str.rxAddr, 0 , 33 );
	nrf_str.sessionSta = 0;
}


//---------------------- 装载数据 ----------------------
//注册设备
void nrf_register_device( void ) {
	uint16_t code;
	if ( nrf_str.sessionSta == 2 ) {
		//注册状态,跳过等待
	} else {
		wait_tx_buff_empty();//等待TX非空
	}
	memset( nrf_str.txBuf, 0, 33 );
	nrf_str.txBuf[0] = DEVICE_ID;
	code = craeteGrowthCode();
	nrf_str.txBuf[1] = code >> 8;
	nrf_str.txBuf[2] = code;
	nrf_str.txBuf[3] = 0;
	memcpy( &nrf_str.txBuf[4], nrf_str.rxAddr, 4 );
	//xTaskNotify( nrf_control_taskHandle, 1U<<NRF_SEND_DATA, eSetBits );
	if ( str.regSta == 3 ) {//心跳
		if ( nrf_send_data() == -1 ) {//通信断开
			str.regSta = 1;
		}
	}
	while( str.regSta == 1 ) {
		if ( nrf_send_data() != -1 ) {//注册成功
			str.regSta = 2;
		}
	}
	//两种情况跳出注册: 1.注册成功 2.按键按下
	
	if ( !str.preKey ) {//注册成功
		LED0_OFF();
	}
	
}

//推送数据
void nrf_push_data( char* pdata ) {
	uint16_t code;
	wait_tx_buff_empty();//等待TX非空
	memset( nrf_str.txBuf, 0, 33 );
	nrf_str.txBuf[0] = DEVICE_ID;
	code = craeteGrowthCode();
	nrf_str.txBuf[1] = code >> 8;
	nrf_str.txBuf[2] = code;
	nrf_str.txBuf[3] = 0x03;//msgType: 3:推送 设备->中转站
	if ( strlen( pdata ) <= 28 ) {
		sprintf( (char*)&nrf_str.txBuf[4], "%s", pdata );
	} else {
		sprintf( (char*)&nrf_str.txBuf[4], "data too long..." );
	}
	xTaskNotify( nrf_control_taskHandle, 1U<<NRF_SEND_DATA, eSetBits );
}

//回复命令
static void nrf_replay_cmd( char* replay ) {
	wait_tx_buff_empty();//等待TX非空
	memset( nrf_str.txBuf, 0, 33 );
	nrf_str.txBuf[0] = DEVICE_ID;
	nrf_str.txBuf[1] = nrf_str.code >> 8;
	nrf_str.txBuf[2] = nrf_str.code;
	nrf_str.txBuf[3] = 2;//msgType: 2:回复 设备->中转站
	if ( strlen( replay ) <= 28 ) {
		sprintf( (char*)&nrf_str.txBuf[4], "%s", replay );
	} else {
		sprintf( (char*)&nrf_str.txBuf[4], "data too long..." );
	}
	xTaskNotify( nrf_control_taskHandle, 1U<<NRF_SEND_DATA, eSetBits );
}

//等待tx空闲
static void wait_tx_buff_empty( void ) {
	uint8_t i;
	for ( i=10; nrf_str.txBuffEmpty!=0&&i>0; i-- ) {
		vTaskDelay( 30 );
	}
	nrf_str.txBuffEmpty = 1;
}


//生成会话code
static uint16_t craeteGrowthCode( void ) {
	static uint16_t code = 0;
	return ++code;
}


void nrf_receive_test(void) {
	uint8_t sta,temp;
	sta = SPI_RW_Reg( NRF_READ_REG + STATUS, NOP );//0xFF空指令
	if ( sta & RX_DR ) {
		SPI_Read_Buf( RD_RX_PLOAD, nrf_str.rxBuf, RX_PLOAD_WIDTH );
		printf("%s\n",nrf_str.rxBuf);
	}
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, 0xF0 );
	SPI_RW_Reg( NRF_WRITE_REG + STATUS, sta );
	HAL_NVIC_EnableIRQ(NRF_IRQ_EXTI_IRQn);
}





