#ifndef __BSP_NRF__H
#define __BSP_NRF__H
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_led.h"

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
#define CD 0x09 				// 地址检测 
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

/*-----------------------------引脚配置------------------------------------*/
//SPI1_GPIO参数
#define SPI1_RCC						RCC_APB2PeriphClockCmd
#define SPI1_CLK						RCC_APB2Periph_SPI1
#define SPI_PORT						SPI1

// \SCK MISO \MOSI \CSN
#define SPI_GPIO_RCC				RCC_APB2PeriphClockCmd
#define SPI_GPIO_CLK				RCC_APB2Periph_GPIOA
#define SPI1_GPIO_PORT			GPIOA
#define SPI1_SCK_GPIOx			GPIO_Pin_5
#define SPI1_MISO_GPIOx			GPIO_Pin_6
#define SPI1_MOSI_GPIOx			GPIO_Pin_7
#define SPI_CSN_RCC					RCC_APB2PeriphClockCmd
#define SPI_CSN_CLK					RCC_APB2Periph_GPIOC
#define SPI_CSN_PORT				GPIOC
#define SPI_CSN_GPIOx				GPIO_Pin_6 // 指南者
//#define SPI_CSN_GPIOx				GPIO_Pin_4 // 最小系统板
// \IRQ \CE
#define NRF_IRQ_PORT				GPIOC			 // 指南者
#define NRF_IRQ_GPIOx				GPIO_Pin_4 // 指南者
#define IRQ_PortSourceGPIOx GPIO_PortSourceGPIOC // 指南者
#define IRQ_PinSource_Pin		GPIO_PinSource4	// 指南者
#define IRQ_EXTI_Lines			EXTI_Line4	// 指南者
#define NRF_IRQHandler			EXTI4_IRQHandler  // 指南者
#define NRF_CE_PORT					GPIOC			  // 指南者
#define NRF_CE_GPIOx				GPIO_Pin_5  // 指南者 

//#define NRF_IRQ_PORT				GPIOC			 // 最小系统板
//#define NRF_IRQ_GPIOx				GPIO_Pin_4 // 最小系统板
//#define IRQ_PortSourceGPIOx GPIO_PortSourceGPIOC // 最小系统板
//#define IRQ_PinSource_Pin		GPIO_PinSource4	// 最小系统板
//#define IRQ_EXTI_Lines			EXTI_Line4	// 最小系统板
//#define NRF_IRQn						TIM4_IRQn		// 最小系统板
//#define NRF_IRQHandler			EXTI4_IRQHandler  // 最小系统板
//#define NRF_CE_PORT				GPIOA			 // 最小系统板
//#define NRF_CE_GPIOx			GPIO_Pin_4 // 最小系统板


/*----------------------------位操作-----------------------------------*/
#define CSN_Low						GPIOC->BRR = GPIO_Pin_6;
#define CSN_High					GPIOC->BSRR = GPIO_Pin_6;
#define SCK_Low						GPIOA->BRR = GPIO_Pin_5;
#define SCK_High					GPIOA->BSRR = GPIO_Pin_5;
#define CE_Low						GPIOC->BRR = GPIO_Pin_5;
#define CE_High						GPIOC->BSRR = GPIO_Pin_5;
#define IRQ_High					GPIOC->BRR = GPIO_Pin_4;
#define IRQ_Low						GPIOC->BSRR = GPIO_Pin_4;
#define TimeOut											10000

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
