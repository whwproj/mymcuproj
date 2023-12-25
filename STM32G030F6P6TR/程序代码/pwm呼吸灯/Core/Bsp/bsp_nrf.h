#ifndef __BSP_NRF__H
#define __BSP_NRF__H

#include "../common.h"

#define DEVICE_ID 1

//********************NRF24L01*********************** ************** 
#define TX_PLOAD_WIDTH 32  // 20 uints TX payload 
#define RX_PLOAD_WIDTH 32  // 20 uints TX payload  
//*****************NRF24L01 寄 存 器 指 令 ***************************
#define NRF_READ_REG 0x00  // 读寄存器指令 
#define NRF_WRITE_REG 0x20 // 写寄存器指令 
#define RD_RX_PLOAD 0x61   // 读取接收数据指令 
#define WR_TX_PLOAD 0xA0   // 写待发数据指令 
#define FLUSH_TX 0xE1 		 // 冲洗发送 FIFO 指令 
#define FLUSH_RX 0xE2 		 // 冲洗接收 FIFO 指令 
#define REUSE_TX_PL 0xE3 	 // 定义重复装载数据指令 
#define NOP 0xFF 			     // 保留 
//******************SPI(nRF24L01) 寄 存 器 地 址 *************** 
#define CONFIG 0x00     // 配置收发状态，CRC 校验模式以及收发状态响应方式 
#define EN_AA 0x01      // 自动应答功能设置 
#define EN_RXADDR 0x02  // 可用信道设置 
#define SETUP_AW 0x03   // 收发地址宽度设置 
#define SETUP_RETR 0x04 // 自动重发功能设置 
#define RF_CH 0x05 			// 工作频率设置 
#define RF_SETUP 0x06 	// 发射速率、功耗功能设置 
#define STATUS 0x07 		// 状态寄存器 
#define OBSERVE_TX 0x08 // 发送监测功能 
#define RPD 0x09 				// 载波检测(功率检测>-65dBm输出高电平)
#define RX_ADDR_P0 0x0A // 频道 0 接收数据地址 
#define RX_ADDR_P1 0x0B // 频道 1 接收数据地址 
#define RX_ADDR_P2 0x0C // 频道 2 接收数据地址 
#define RX_ADDR_P3 0x0D // 频道 3 接收数据地址 
#define RX_ADDR_P4 0x0E // 频道 4 接收数据地址 
#define RX_ADDR_P5 0x0F // 频道 5 接收数据地址
#define TX_ADDR 0x10 		// 发送地址寄存器 
#define RX_PW_P0 0x11	  // 接收频道 0 接收数据长度 
#define RX_PW_P1 0x12   // 接收频道 0 接收数据长度 
#define RX_PW_P2 0x13   // 接收频道 0 接收数据长度 
#define RX_PW_P3 0x14   // 接收频道 0 接收数据长度 
#define RX_PW_P4 0x15   // 接收频道 0 接收数据长度 
#define RX_PW_P5 0x16   // 接收频道 0 接收数据长度 
#define FIFO_STATUS 0x17// FIFO 栈入栈出状态寄存器设置 
#define DYNPD			0x1C	//动态载荷长度
#define FEATURE		0x1D	//动态长度,自动确认,自动应答
//****************** STATUS 状态值 *************** 
#define	RX_DR			1<<6	//RX FIFO interrupt
#define TX_DS			1<<5	//TX FIFO interrupt
#define MAX_RT		1<<4	//重发失败达到最大次数中断
#define RX_P_NO_0	0<<1	//数据通道0
#define RX_P_NO_1	1<<1	//数据通道1
#define RX_P_NO_2	2<<1	//数据通道2
#define RX_P_NO_3	3<<1	//数据通道3
#define RX_P_NO_4	4<<1	//数据通道4
#define RX_P_NO_5	5<<1	//数据通道5
#define TX_FULL		1<<0	//1: TX FIFO full.


#define NRF_SPI_Handle 	hspi1

/*---------------------------- 位操作 -----------------------------------*/
#define CSN_Low()					NRF_CSN_GPIO_Port->BRR = NRF_CSN_Pin
#define CSN_High()					NRF_CSN_GPIO_Port->BSRR = NRF_CSN_Pin
#define CE_Low()						NRF_CE_GPIO_Port->BRR = NRF_CE_Pin
#define CE_High()						NRF_CE_GPIO_Port->BSRR = NRF_CE_Pin
#define IRQ_isHigh()				(NRF_IRQ_GPIO_Port->IDR)&NRF_IRQ_Pin
#define IRQ_isLow()					!((NRF_IRQ_GPIO_Port->IDR)&NRF_IRQ_Pin)
/*--------------------------- 位操作 ---------------------------------*/

/*-------------------- Task start --------------------*/
#define NRF_INIT_EVENT									0
#define NRF_SEND_DATA									1
#define NRF_PARSE_DATA								2
#define NRF_REGISTER_DEVICE					3
#define NRF_STOP_REGISTER_DEVICE		4
#define NRF_HEARTBEAT									5
#define SEND_KEY_DOWN								6
/*-------------------- Task end --------------------*/



/** @defgroup CONFIG_Define 工作模式配置寄存器
  * @{
  */
/* ---------------------------------------------------------------------- */
#define MASK_RX_DR   1<<6		/* 1:屏蔽RX_DS中断, 以下几种情况会触发RX_DS事件
															RX: 1> (没有使能自动确认)收到数据后触发
																	2> (使能自动确认功能)收到数据后,校验为新数据后触发
															TX: 1> (使能自动重发功能,发送非NO_ACK包),收到应答ACK后触发	*/
/* ---------------------------------------------------------------------- */
#define MASK_TX_DS   1<<5		/* 1:屏蔽TX_DS中断, 以下几种情况会触发TX_DS事件
															TX: 1> (没有使能自动重发)发送数据后会触发
																	2> (使能自动重发,发送NO_ACK包),发送数据后会触发
																	3> (使能自动重发,非NO_ACK包),发送数据后,收到ACK应答  
															RX: 1> (使能自动确认功能)收到数据后,应答ACK,ACK中附带有效载荷	*/
/* ---------------------------------------------------------------------- */
#define MASK_MAX_RT  1<<4		/* 1:屏蔽最大重发次数中断    */
/* ---------------------------------------------------------------------- */
#define EN_CRC       1<<3		/* 使能CRC(必须也使能自动确认功能) 1:使能   */
/* ---------------------------------------------------------------------- */
#define CRCO				 1<<2		/* CRC字节数 0:1byte 1:2byte    */
/* ---------------------------------------------------------------------- */
#define PWR_UP       1<<1		/* 1:POWER UP 0:POWER DOWN    */
/* ---------------------------------------------------------------------- */
#define PRIM_RX      1<<0		/* 1:RX 0:TX    */
/* ---------------------------------------------------------------------- */


/** @defgroup EN_AA_Define 使能RX自动确认功能(Enhanced ShockBurst)
  * @{
  */
#define ENAA_P5			1<<5		/* 使能RX通道5自动确认 1:使能 */
#define ENAA_P4			1<<4		/* 使能RX通道4自动确认 1:使能 */
#define ENAA_P3			1<<3		/* 使能RX通道3自动确认 1:使能 */
#define ENAA_P2			1<<2		/* 使能RX通道2自动确认 1:使能 */
#define ENAA_P1			1<<1		/* 使能RX通道1自动确认 1:使能 */
#define ENAA_P0			1<<0		/* 使能RX通道0自动确认 1:使能 */


/** @defgroup EN_RXADDR_Define 使能接收通道
  * @{
  */
#define ERX_P5			1<<5		/* 使能接收通道5 1:使能 */
#define ERX_P4			1<<4		/* 使能接收通道4 1:使能 */
#define ERX_P3			1<<3		/* 使能接收通道3 1:使能 */
#define ERX_P2			1<<2		/* 使能接收通道2 1:使能 */
#define ERX_P1			1<<1		/* 使能接收通道1 1:使能 */
#define ERX_P0			1<<0		/* 使能接收通道0 1:使能 */


/** @defgroup SETUP_AW_Define 设置地址宽度[3-5]byte
  * @{
  */
#define AW_WIDTH_3_BYTE		1	/* 地址宽度3byte */
#define AW_WIDTH_4_BYTE		2	/* 地址宽度4byte */
#define AW_WIDTH_5_BYTE		3	/* 地址宽度5byte */


/** @defgroup SETUP_RETR_Define 设置自动重传
  * @{
  */
#define AUTO_RETRANSMIT_DELAY_US(x) 	(x/250-1)	/* 自动重传时间250us整数倍,最大4000us,常用500us */
#define AUTO_RETRANSMIT_COUNT(x)			(x)				/* 自动重发次数 */


/** @defgroup RF_SETUP_Define 射频设置寄存器
  * @{
  */
/* ---------------------------------------------------------------------- */
#define CONT_WAVE				1<<7/* 连续载波传输,传输的不是数据,而是信号,仅用于保持通信通道打开,一般不用 */
/* ---------------------------------------------------------------------- */
#define RF_DR_1Mbps			0x00/* 射频速率 1Mbps		xx(0)x(0)xxx */
#define RF_DR_2Mbps			0x08/* 射频速率 2Mbps		xx(0)x(1)xxx */
#define RF_DR_250kbps		0x20/* 射频速率 250kbps	xx(1)x(0)xxx */
/* ---------------------------------------------------------------------- */
#define RF_PWR_n18dB		0x00/* 发射功率-18dBm	*/
#define RF_PWR_n12dB		0x02/* 发射功率-12dBm	*/
#define RF_PWR_n6dB			0x04/* 发射功率-6dBm		*/
#define RF_PWR_0dB			0x06/* 发射功率 0dBm		*/
/* ---------------------------------------------------------------------- */


/** @defgroup STATUS_Define 中断状态寄存器
  * @{
  */
#define RX_DR						1<<6						/* RX FIFO收到数据标志,写1清除 */
#define TX_DS						1<<5						/* TX FIFO发送数据标志,写1清除 */
#define MAX_RT					1<<4						/* 达到最大重发次数标志,写1清除 */
#define RX_P_NOx(reg)		((reg&0x0E)>>1) /* (只读)显示来自(0-5)哪一数据管道的数据 */
#define TX_FULL					1<<0						/* (只读)TX FIFO非空标志,需要先清空缓存或等待发送 */
	

/** @defgroup OBSERVE_TX_Define 发送失败计数寄存器
  * @{
  */
#define PLOS_CNTx(reg)	((reg&0xF0)>>4) /* (只读)计数发送失败数据包次数,通过写入RF_CH重置计数器 */
#define ARC_CNTx(reg)	((reg&0x0F)) 			/* (只读)计数一次重传的次数,新数据开始传输重置 */
	

/** @defgroup FIFO_STATUS_Define FIFO状态寄存器
  * @{
  */
#define TX_REUSE			1<<6 /* TX_REUSE载荷重用由SPI命令REUSE_TX_PL设置,通过SPI命令W_TX_PAYLOAD或FLUSH_TX重置 */			
#define TX_FULL_2			1<<5 /* TX FIFO非空标志,需要清空缓存或等待发送 */
#define TX_EMPTY			1<<4 /* TX FIFO空标志 */
#define RX_FULL				1<<1 /* RX FIFO非空标志,需要接收数据或清空缓存 */
#define RX_EMPTY			1<<0 /* RX FIFO空标志 */


/** @defgroup DYNPD_Define (配置接收端)使能动态长度
  * @{
  */
#define DPL_P5	1<<5 /* 使能数据通道5动态数据长度 */
#define DPL_P4	1<<4 /* 使能数据通道4动态数据长度 */
#define DPL_P3	1<<3 /* 使能数据通道3动态数据长度 */
#define DPL_P2	1<<2 /* 使能数据通道2动态数据长度 */
#define DPL_P1	1<<1 /* 使能数据通道1动态数据长度 */
#define DPL_P0	1<<0 /* 使能数据通道0动态数据长度 */
	
	
/** @defgroup FEATURE_Define (配置发送端)使能动态长度2
  * @{
  */
#define EN_DPL			1<<2 /* 使能发送端启用动态数据长度报文格式 */
#define EN_ACK_PAYd	1<<1 /* 使能TX端对RX有效载荷的自动确认 */
#define EN_DYN_ACK	1<<0 /* 使能该条数据不需要自动确认(NO_ACK) */


//中转站地址 0x1A,0x2B,0x3C,0x4D
typedef struct _NRF_STR {
	uint16_t code;
	//uint8_t deviceReg;//0:不注册 1:开启设备注册 2:已注册
	//uint8_t cfgKey;//按键状态 0:未按下 1:按下
	//uint8_t sessionSta;//0:默认状态 1:接收命令状态 2:注册状态
	//uint8_t txBuffEmpty;//0:txBuff为空 1:txBuff非空,正在发送数据
	uint8_t heartTime;//注册成功后空闲计时,10s后发一次心跳包
	uint8_t localAddr[4];//本机收发地址(NRF增强模式中收发地址要一致)
	uint8_t TSAddr[4];//中转站地址(NRF增强模式中收发地址要一致)
	uint8_t *txBuf;
	uint8_t *rxBuf;
	uint8_t regSta;//0:不注册 1:正在注册 2:已注册 3:心跳 4:通信成功,等待注册结果
	uint8_t reg4Num;//等待注册反馈计时
	//uint8_t deviceList[100];//在线设备生命周期,心跳重置10,递减到0则离线
	
} NRF_STR;


/*NRF初始化配置*/
typedef struct {
	uint8_t CONFIG_;			/*!< @ref CONFIG_Define 工作模式配置寄存器 */
	
	uint8_t EN_AA_;				/*!< @ref EN_AA_Define 使能自动确认功能(Enhanced ShockBurst) */
	
	uint8_t EN_RXADDR_;		/*!< @ref EN_RXADDR_Define 使能接收通道(0~5) */
	
	uint8_t SETUP_AW_;		/*!< @ref SETUP_AW_Define 设置地址宽度[3-5]byte */
	
	uint8_t SETUP_RETR_;	/*!< @ref SETUP_RETR_Define 设置自动重传 */
	
	uint8_t	RF_CH_;				/* 射频频率,低7位设置,[0-127]1代表1MHz 2400MHz~2525MHz	*/
	
	uint8_t RF_SETUP_;		/*!< @ref RF_SETUP_Define 射频设置寄存器 */
	
	uint8_t STATUS_;			/*!< @ref STATUS_Define 中断状态寄存器 */ 					/**** 可读可写,清除标志 ****/
	
	uint8_t OBSERVE_TX_;	/*!< @ref OBSERVE_TX_Define 发送失败计数寄存器 */ 	/**** 仅可读 ****/
	
	uint8_t RPD_;					/* 可用于检测信号强度	*/													/**** 仅可读 ****/
	
	uint8_t RX_ADDR_P0_[5];	/* 通道0地址 */
	uint8_t RX_ADDR_P1_[5];	/* 通道1地址 */
	uint8_t RX_ADDR_P2_[5];	/* 通道2地址 */
	uint8_t RX_ADDR_P3_[5];	/* 通道3地址 */
	uint8_t RX_ADDR_P4_[5];	/* 通道4地址 */
	uint8_t RX_ADDR_P5_[5];	/* 通道5地址 */
	
	uint8_t TX_ADDR_[5];	/* 发送地址 */
	
	uint8_t	RX_PW_P0_;		/* 通道0静态有效载荷长度 [1-32] (使能动态长度不用设置) */
	uint8_t	RX_PW_P1_;		/* 通道1静态有效载荷长度 [1-32] (使能动态长度不用设置) */
	uint8_t	RX_PW_P2_;		/* 通道2静态有效载荷长度 [1-32] (使能动态长度不用设置) */
	uint8_t	RX_PW_P3_;		/* 通道3静态有效载荷长度 [1-32] (使能动态长度不用设置) */
	uint8_t	RX_PW_P4_;		/* 通道4静态有效载荷长度 [1-32] (使能动态长度不用设置) */
	uint8_t	RX_PW_P5_;		/* 通道5静态有效载荷长度 [1-32] (使能动态长度不用设置) */
	
	uint8_t FIFO_STATUS_;	/*!< @ref FIFO_STATUS_Define FIFO状态寄存器 */			/**** 仅可读 ****/
	
	uint8_t DYNPD_;				/*!< @ref DYNPD_Define 使能动态长度 */
	
	uint8_t FEATURE_;			/*!< @ref FEATURE_Define 使能动态长度2 */

} NRF24L01_TypeDef;


extern NRF_STR nrf_str;

uint8_t SPI_RW_Reg( uint8_t reg, uint8_t value );//读取寄存器
uint8_t SPI_Write_Buf( uint8_t reg, uint8_t *pBuf, uint8_t len );//多字节写入
uint8_t SPI_Read_Buf( uint8_t reg, uint8_t *pBuf, uint8_t len );//多字节读取
uint8_t Nrf24l01_Init( NRF24L01_TypeDef* nrf );
void Tx_Mode( void );
void Rx_Mode( void );

void nrf_init( void );
int nrf_send_data( void );
void nrf_register_device( void );
void nrf_parse_data( void );//解析数据
void nrf_send_heartbeat( void );
void nrf_replay_cmd( char* replay );
void nrf_push_data( char* pdata );//推送数据

#endif /*__BSP_NRF__H*/


