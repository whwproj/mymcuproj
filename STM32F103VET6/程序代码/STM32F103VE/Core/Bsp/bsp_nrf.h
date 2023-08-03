#ifndef __BSP_NRF__H
#define __BSP_NRF__H

#include "../common.h"

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


/*----------------------------位操作-----------------------------------*/
#define CSN_Low()						NRF_CSN_GPIO_Port->BRR = NRF_CSN_Pin
#define CSN_High()					NRF_CSN_GPIO_Port->BSRR = NRF_CSN_Pin
#define CE_Low()						NRF_CE_GPIO_Port->BRR = NRF_CE_Pin
#define CE_High()						NRF_CE_GPIO_Port->BSRR = NRF_CE_Pin
#define IRQ_isHigh()			(NRF_IRQ_GPIO_Port->IDR)&NRF_IRQ_Pin
#define IRQ_isLow()				!((NRF_IRQ_GPIO_Port->IDR)&NRF_IRQ_Pin)


/*---------------------------应用函数---------------------------------*/
void NRF_Init(void);
void Rx_Mode(void);
void NRF_RX_ITRPT(void);
void Tx_Mode(uint8_t temp);
void NRF_TX_ITRPT(void);
void CleanStatus(uint8_t temp); // 清除标志位
uint8_t ReadStatus(void);	// 读取标志位
void ReadSuccess(void); // 接收数据成功操作
void SendMax(void);	// 发送次数最大操作
void SendDatAndSuccess(void);	// 发送数据或检测成功
//void TxBuf_TxBufx(uint8_t *TxBufs); //	交换数据
void USART_Receive(void);		// 串口接收数据
void DisplayDat(void);	// 显示发送数据
#endif /*__BSP_NRF__H*/
