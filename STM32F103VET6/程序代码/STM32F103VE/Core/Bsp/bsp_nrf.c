#include "../Bsp/bsp_nrf.h"

uint8_t Long;
//uint8_t TX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01}; //本地地址 
//uint8_t RX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01}; //接收地址
uint8_t TX_ADDRESS[5] = {0xA3,0xA3,0xA3,0xA3,0xA3}; //本地地址 
uint8_t RX_ADDRESS[5] = {0xA3,0xA3,0xA3,0xA3,0xA3}; //接收地址
uint8_t tx_buf[]={"NRF24L01增强模式测试成功！\n"};
uint8_t rx_buf[224];

static uint8_t SPI2_RW_Reg(uint8_t reg, uint8_t value);
static uint8_t SPI2_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
static uint8_t SPI2_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);

uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value) {//读取寄存器
	uint8_t status; 
	CSN_Low(); 
	HAL_SPI_Transmit( &hspi1, &reg, 1, 100 );
	HAL_SPI_TransmitReceive( &hspi1, &value, &status, 1, 100 );
	CSN_High();
	return(status);
} 

static uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars) { 
	uint8_t status; 
	CSN_Low();  
	HAL_SPI_TransmitReceive( &hspi1, &reg, &status, 1, 100 );
	HAL_SPI_Transmit( &hspi1, pBuf, uchars, 100 );
	CSN_High(); 
	return(status);  
}  

static uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars) { 
	uint8_t status; 
	CSN_Low();   
	HAL_SPI_TransmitReceive( &hspi1, &reg, &status, 1, 100 );
	HAL_SPI_Receive( &hspi1, pBuf, uchars, 100);
	CSN_High(); 
	return(status); // return nRF24L01 status uchar 
}

void NRF_Init(void) {
	uint8_t status;
	vTaskDelay(100);
	CE_Low();
	status = SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);//RX,数据通道0,使能增强TM自动确认功能
	status = SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);//RX,使能数据管道0;
	status = SPI_RW_Reg(NRF_WRITE_REG + SETUP_AW, 0x03);//管道地址宽度5Byte
	status = SPI_RW_Reg(NRF_WRITE_REG + SETUP_RETR, 0x01);//自动重发时间500us
	status = SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 0x02);//Fo = 2400 + RF_CH(MHz) 2400MHz ~ 2525MHz
	status = SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x06);//发射功率0db,速率1Mbps,
	//STATUS: SPI_RW_Reg(NRF_WRITE_REG + STATUS, 1<<4,5,6);//状态寄存器,中断清除
	//OBSERVE_TX:数据包丢失寄存器(只读)
	//RPD: 载波检测(功率检测>-65dBm输出高电平)
	status = SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5);//配置信道地址
	status = SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5);//配置信道地址
	
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, 0x20);//静态有效载荷长度32字节
	//FIFO_STATUS: 获取FIFO是否存在数据状态等
	//status = SPI_RW_Reg(NRF_WRITE_REG + DYNPD, 0x01);//通道0,使能动态载荷长度
	//status = SPI_RW_Reg(NRF_WRITE_REG + FEATURE, 0x06);//使能动态长度EN_DPL=1,开启有效载荷EN_ACK_PAY=1,NOACK=0
	//0	0	0		1												1				0							1					1			
	//				MAX_RT(最大重发不中断)		EN_CRC	CRC(1Byte)		POWER_UP	PRX 
	status = SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x3B);//屏蔽TX,MAX_RT中断,CRC1Byte,RX模式
	SPI_RW_Reg(FLUSH_TX,NOP);//清空FIFO
	SPI_RW_Reg(FLUSH_RX,NOP);//清空FIFO
	SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xF0);
	CE_High();
}
void nrf_receive_data(void) {
	uint8_t sta,temp;
	sta = SPI_RW_Reg(NRF_READ_REG + STATUS, NOP);//0xFF空指令
	if ( sta & RX_DR ) {
		SPI_Read_Buf(RD_RX_PLOAD, rx_buf, RX_PLOAD_WIDTH);
		printf("%s\n",rx_buf);
		//SPI_RW_Reg(FLUSH_RX,NOP);
		SPI_RW_Reg(NRF_WRITE_REG + STATUS, sta);
	}
	
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void Tx_Mode(uint8_t temp) {
	CE_Low();
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x1A);//TX模式,POWER_UP=1,CRC1byte,EN_CRC,最大重发不中断
	SPI_Write_Buf(WR_TX_PLOAD , (tx_buf + temp) , 32);
	CE_High();
}
void Rx_Mode(void) {
	CE_Low(); // chip enable
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, 5);  // 接收设备接收通道0使用和发送设备相同的发送地址
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);               	    	// 使能接收通道0自动应答
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);           			// 使能接收通道0
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 1);                 			// 选择射频通道0x40
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);  			// 接收通道0选择和发送通道相同有效数据宽度
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);            			// 数据传输率2Mbps，发射功率0dBm，低噪声放大器增益(nRF24L01+忽略该位）
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f);              			// CRC使能，16位CRC校验，上电，接收模式
	SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xff);  						// 清除所有的中断标志位
	CE_High(); 
}

void CleanStatus(uint8_t temp) {// 清除标志位
	CSN_Low(); 
	SPI_RW_Reg(NRF_WRITE_REG+STATUS,temp); 
	CSN_High();
}
void CleanIT(void) {
	CE_Low(); // chip enable 
	CSN_High(); // Spi disable 
	//SCK = 0; // Spi clock line init high 
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5); 
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5);  
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01); 
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01); 
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 1);
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x07); 
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0xFf); 
	CE_High();
}

void SendDatAndSuccess(void) {// 发送完成操作
	uint8_t a,b;
	a = Long / 32;
	b = Long % 32;
	//delay(0xFFFF);
	vTaskDelay(1000);
	CE_Low();
	SPI_RW_Reg(FLUSH_TX,NOP);
	CleanStatus(0xff);
	if((a==0&&b!=0)||(a==1&&b==0))	//	1页能写完
	{
		NRF_Init();
		Tx_Mode(0);
		Long = 0;
	}
	else if(a>1)	// 1页写不完
	{
		static uint8_t temp=0;
		if(temp<a)
		{
			NRF_Init();
			Tx_Mode(32*temp);
			temp++;
		}
		if(b!=0&&temp==a)
		{
			NRF_Init();
			Tx_Mode(32*temp+b);
			Long = 0;
		}
		if(temp>=a)
		{
			Long = 0;
		}
	}
	if(Long==0&&a==0&&b==0)
	{
		printf("          ------收到应答，发送成功------\n");
		CE_Low();
		SPI_RW_Reg(FLUSH_TX,NOP);
		CleanStatus(0xff);
		CleanIT();
		NRF_Init();
		Rx_Mode();
	}
}
void SendMax(void) {// 发送次数最大操作
	CE_Low();
	SPI_RW_Reg(FLUSH_TX,NOP);
	CleanStatus(0xff);
	printf("          ------应答超时，发送失败------\n");
	CleanIT();
	NRF_Init();
	Rx_Mode();
}

static uint8_t SPI2_RW_Reg(uint8_t reg, uint8_t value) {// 读写函数 
	uint8_t status; 
	CSN2_Low(); 
	HAL_SPI_Transmit( &hspi2, &reg, 1, 100 );
	HAL_SPI_TransmitReceive( &hspi2, &value, &status, 1, 100 );
	CSN2_High();
	return(status);
} 
static uint8_t SPI2_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars) { 
	uint8_t status;
	CSN2_Low();
	HAL_SPI_TransmitReceive( &hspi2, &reg, &status, 1, 100 );
	HAL_SPI_Transmit( &hspi2, pBuf, uchars, 100 );
	CSN2_High();
	return(status);
} 
static uint8_t SPI2_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars) { 
	uint8_t status; 
	CSN2_Low();   
	HAL_SPI_TransmitReceive( &hspi2, &reg, &status, 1, 100 );
	HAL_SPI_Receive( &hspi2, pBuf, uchars, 100);
	CSN2_High(); 
	return(status); // return nRF24L01 status uchar 
}
void NRF2_Init(void) {
	uint8_t status;
	vTaskDelay(100);
	CE2_Low();
	status = SPI2_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);//RX,数据通道0,使能增强TM自动确认功能
	status = SPI2_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);//RX,使能数据管道0;
	status = SPI2_RW_Reg(NRF_WRITE_REG + SETUP_AW, 0x03);//管道地址宽度5Byte
	status = SPI2_RW_Reg(NRF_WRITE_REG + SETUP_RETR, 0x01);//自动重发时间500us
	status = SPI2_RW_Reg(NRF_WRITE_REG + RF_CH, 0x02);//Fo = 2400 + RF_CH(MHz) 2400MHz ~ 2525MHz
	status = SPI2_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x06);//发射功率0db,速率1Mbps,
	//STATUS: SPI2_RW_Reg(NRF_WRITE_REG + STATUS, 1<<4,5,6);//状态寄存器,中断清除
	//OBSERVE_TX:数据包丢失寄存器(只读)
	//RPD: 载波检测(功率检测>-65dBm输出高电平)
	status = SPI2_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5);//配置信道地址
	status = SPI2_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5);//配置信道地址
	
	SPI2_Read_Buf( TX_ADDR, rx_buf, 5 );
	
	SPI2_RW_Reg(NRF_WRITE_REG + RX_PW_P0, 0x20);//静态有效载荷长度32字节
	//FIFO_STATUS: 获取FIFO是否存在数据状态等
	//status = SPI2_RW_Reg(NRF_WRITE_REG + DYNPD, 0x01);//通道0,使能动态载荷长度
	//status = SPI2_RW_Reg(NRF_WRITE_REG + FEATURE, 0x06);//使能动态长度EN_DPL=1,开启有效载荷EN_ACK_PAY=1,NOACK=0
	//0	0	0		1												1				0							1					1			
	//				MAX_RT(最大重发不中断)		EN_CRC	CRC(1Byte)		POWER_UP	PRX 
	status = SPI2_RW_Reg(NRF_WRITE_REG + CONFIG, 0x3A);//屏蔽TX,MAX_RT中断,CRC1Byte,TX模式
	SPI_RW_Reg(FLUSH_TX,NOP);//清空FIFO
	SPI_RW_Reg(FLUSH_RX,NOP);//清空FIFO
	CE2_High();
}

void Tx2_Mode( void ) {
	SPI2_Write_Buf(WR_TX_PLOAD , tx_buf , 32);
}
