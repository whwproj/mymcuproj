#include "bsp_nrf.h"

uint8_t Long;
uint8_t TX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01}; //本地地址 
uint8_t RX_ADDRESS[5] = {0x34,0x43,0x10,0x10,0x01}; //接收地址
uint8_t TxBuf[224];//={"NRF24L01增强模式测试成功！\n"};
uint8_t rx_buf[224];
static void delay(u32 temp)
{
	while(temp--);
}
static void NRF_NVIC(void)
{
	
	NVIC_InitTypeDef NVIC_InitStructuer;
	
	NVIC_InitStructuer.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructuer.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructuer.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructuer.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructuer);
	
}
static void SPI_GPIO_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructuer;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	SPI_CSN_RCC(SPI_CSN_CLK,ENABLE);
	SPI_GPIO_RCC(SPI_GPIO_CLK,ENABLE);
	NRF_NVIC();
	// \CSN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SPI_CSN_GPIOx;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_CSN_PORT,&GPIO_InitStructure);
	CSN_High;
	// \SCK \MISO \MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = SPI1_SCK_GPIOx|SPI1_MISO_GPIOx|SPI1_MOSI_GPIOx;
	GPIO_Init(SPI1_GPIO_PORT,&GPIO_InitStructure);
	// \CE \IRQ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = NRF_CE_GPIOx;
	GPIO_Init(NRF_CE_PORT,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_GPIOx;
	GPIO_Init(NRF_IRQ_PORT,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(IRQ_PortSourceGPIOx, IRQ_PinSource_Pin);
	EXTI_InitStructuer.EXTI_Line = IRQ_EXTI_Lines;
	EXTI_InitStructuer.EXTI_LineCmd = ENABLE;
	EXTI_InitStructuer.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructuer.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructuer);
	
}

static void SPI_Config(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	/*GPIO*/
	SPI_GPIO_Config();
	FLASH_SPI_CS_HIGH(); 
	SPI1_RCC(SPI1_CLK,ENABLE);
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;/*设置时钟分频因子， fpclk/分频数=fSCK */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;/*SPI_CPHA_2Edge;*///SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;/*SPI_CPOL_High;*///SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 0;/*设置 CRC 校验的表达式 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;/*设置 SPI 的单双向模式 */
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;/*设置 MSB/LSB 先行 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;/*设置 SPI 的主/从机端模式 */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;/*设置 NSS 引脚由 SPI 硬件控制还是软件控制*/
	SPI_Init(SPI_PORT,&SPI_InitStructure);
	SPI_Cmd(SPI_PORT,ENABLE);
}

static u16 NRF_SPI_WriteByte(u16 dat)
{
	u16 time;
	time = TimeOut;
	while(SPI_I2S_GetFlagStatus(SPI_PORT,SPI_I2S_FLAG_TXE) != SET)
	{
		time--;if(!time){printf("等待TXE超时");return 0;}
	}
	//SPI_I2S_ClearFlag(SPI_PORT,SPI_I2S_FLAG_TXE);
	SPI_I2S_SendData(SPI_PORT,dat);
	time = TimeOut;
	while(SPI_I2S_GetFlagStatus(SPI_PORT,SPI_I2S_FLAG_RXNE) != SET)
	{
		time--;if(!time){printf("等待RXNE超时");return 0;}
	}
	//SPI_I2S_ClearFlag(SPI_PORT,SPI_I2S_FLAG_RXNE);
	time = SPI_I2S_ReceiveData(SPI_PORT);
	return (time);
}

static uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value)	// 读写函数 
{ 
	uint8_t status; 
	CSN_Low; 
	NRF_SPI_WriteByte(reg); 
	status = NRF_SPI_WriteByte(value); 
	CSN_High;
	return(status); 
} 

static u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars) 
{ 
	u8 status,uchar_ctr; 
	CSN_Low;  
	status = NRF_SPI_WriteByte(reg); 
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) 
	NRF_SPI_WriteByte(*pBuf++); 
	CSN_High; 
	return(status);  
}  

static u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars) 
{ 
	u8 status,uchar_ctr; 
	CSN_Low;   
	status = NRF_SPI_WriteByte(reg); 
	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++) 
	pBuf[uchar_ctr] = NRF_SPI_WriteByte(0); 
	CSN_High; 
	return(status); // return nRF24L01 status uchar 
}


void NRF_Init(void)
{
	SPI_Config();
	CE_Low; // chip enable 
	CSN_High; // Spi disable 
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5); 
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5);  
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01); 
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01); 
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 1);
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x07); 
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f); 
	CE_High;
}
void NRF_RX_ITRPT(void)
{
	SPI_Read_Buf(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);
	printf("%s\n",rx_buf);
}

void Tx_Mode(uint8_t temp)
{
	CE_Low; // chip enable 
	CSN_High; // Spi disable 
	//SCK = 0; // Spi clock line init high 
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5); 
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5); 
	SPI_Write_Buf(WR_TX_PLOAD , (TxBuf + temp) , 32);
	CE_High;
}
void Rx_Mode(void)
{
	CE_Low; // chip enable
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, 5);  // 接收设备接收通道0使用和发送设备相同的发送地址
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);               	    	// 使能接收通道0自动应答
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);           			// 使能接收通道0
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 1);                 			// 选择射频通道0x40
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);  			// 接收通道0选择和发送通道相同有效数据宽度
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x0f);            			// 数据传输率2Mbps，发射功率0dBm，低噪声放大器增益(nRF24L01+忽略该位）
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f);              			// CRC使能，16位CRC校验，上电，接收模式
	SPI_RW_Reg(NRF_WRITE_REG + STATUS, 0xff);  						// 清除所有的中断标志位
	CE_High; 
}

uint8_t ReadStatus(void)	// 读取标志位
{
	return (SPI_RW_Reg(NRF_READ_REG + STATUS, NOP));
}
void CleanStatus(uint8_t temp) // 清除标志位
{
	CSN_Low; 
	SPI_RW_Reg(NRF_WRITE_REG+STATUS,temp); 
	CSN_High;
}
void CleanIT(void)
{
	CE_Low; // chip enable 
	CSN_High; // Spi disable 
	//SCK = 0; // Spi clock line init high 
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, 5); 
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, 5);  
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01); 
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01); 
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 1);
	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x07); 
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0xFf); 
	CE_High;
}
void ReadSuccess(void) // 接收数据成功操作
{
	NRF_RX_ITRPT();
	//CE_Low;
	SPI_RW_Reg(FLUSH_RX,NOP);
	CleanStatus(0xff);
	//NRF_Init();
	//SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f); 
	CE_High;
	//Rx_Mode();
}
void SendDatAndSuccess(void) // 发送完成操作
{
	uint8_t a,b;
	a = Long / 32;
	b = Long % 32;
	delay(0xFFFF);
	CE_Low;
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
		CE_Low;
		SPI_RW_Reg(FLUSH_TX,NOP);
		CleanStatus(0xff);
		CleanIT();
		NRF_Init();
		Rx_Mode();
	}
}
void SendMax(void)	// 发送次数最大操作
{
	CE_Low;
	SPI_RW_Reg(FLUSH_TX,NOP);
	CleanStatus(0xff);
	printf("          ------应答超时，发送失败------\n");
	CleanIT();
	NRF_Init();
	Rx_Mode();
}
void DisplayDat(void)	// 显示发送数据
{
	*TxBuf = USART_ReceiveData(DEBUG_USARTx);
	printf("将要发送的数据%s\n",TxBuf);
}
void USART_Receive(void)
{
	static uint8_t num;
	uint8_t temp;
	temp = USART_ReceiveData(USART1);
	if((num>=20)|(temp==0x0D))
	{
		Long = num;
		num=0;
		//DisplayDat();
		SendDatAndSuccess();
	}
	else{TxBuf[num] = temp;num++;}
	
}
