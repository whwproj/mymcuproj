#ifndef __BSP_NRF__H
#define __BSP_NRF__H

#include "../common.h"

//********************NRF24L01*********************** ************** 
#define TX_PLOAD_WIDTH 32  // 20 uints TX payload 
#define RX_PLOAD_WIDTH 32  // 20 uints TX payload  
//*****************NRF24L01 �� �� �� ָ �� ***************************
#define NRF_READ_REG 0x00  // ���Ĵ���ָ�� 
#define NRF_WRITE_REG 0x20 // д�Ĵ���ָ�� 
#define RD_RX_PLOAD 0x61   // ��ȡ��������ָ�� 
#define WR_TX_PLOAD 0xA0   // д��������ָ�� 
#define FLUSH_TX 0xE1 		 // ��ϴ���� FIFO ָ�� 
#define FLUSH_RX 0xE2 		 // ��ϴ���� FIFO ָ�� 
#define REUSE_TX_PL 0xE3 	 // �����ظ�װ������ָ�� 
#define NOP 0xFF 			     // ���� 
//******************SPI(nRF24L01) �� �� �� �� ַ *************** 
#define CONFIG 0x00     // �����շ�״̬��CRC У��ģʽ�Լ��շ�״̬��Ӧ��ʽ 
#define EN_AA 0x01      // �Զ�Ӧ�������� 
#define EN_RXADDR 0x02  // �����ŵ����� 
#define SETUP_AW 0x03   // �շ���ַ������� 
#define SETUP_RETR 0x04 // �Զ��ط��������� 
#define RF_CH 0x05 			// ����Ƶ������ 
#define RF_SETUP 0x06 	// �������ʡ����Ĺ������� 
#define STATUS 0x07 		// ״̬�Ĵ��� 
#define OBSERVE_TX 0x08 // ���ͼ�⹦�� 
#define RPD 0x09 				// �ز����(���ʼ��>-65dBm����ߵ�ƽ)
#define RX_ADDR_P0 0x0A // Ƶ�� 0 �������ݵ�ַ 
#define RX_ADDR_P1 0x0B // Ƶ�� 1 �������ݵ�ַ 
#define RX_ADDR_P2 0x0C // Ƶ�� 2 �������ݵ�ַ 
#define RX_ADDR_P3 0x0D // Ƶ�� 3 �������ݵ�ַ 
#define RX_ADDR_P4 0x0E // Ƶ�� 4 �������ݵ�ַ 
#define RX_ADDR_P5 0x0F // Ƶ�� 5 �������ݵ�ַ
#define TX_ADDR 0x10 		// ���͵�ַ�Ĵ��� 
#define RX_PW_P0 0x11	  // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P1 0x12   // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P2 0x13   // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P3 0x14   // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P4 0x15   // ����Ƶ�� 0 �������ݳ��� 
#define RX_PW_P5 0x16   // ����Ƶ�� 0 �������ݳ��� 
#define FIFO_STATUS 0x17// FIFO ջ��ջ��״̬�Ĵ������� 
#define DYNPD			0x1C	//��̬�غɳ���
#define FEATURE		0x1D	//��̬����,�Զ�ȷ��,�Զ�Ӧ��
//****************** STATUS ״ֵ̬ *************** 
#define	RX_DR			1<<6	//RX FIFO interrupt
#define TX_DS			1<<5	//TX FIFO interrupt
#define MAX_RT		1<<4	//�ط�ʧ�ܴﵽ�������ж�
#define RX_P_NO_0	0<<1	//����ͨ��0
#define RX_P_NO_1	1<<1	//����ͨ��1
#define RX_P_NO_2	2<<1	//����ͨ��2
#define RX_P_NO_3	3<<1	//����ͨ��3
#define RX_P_NO_4	4<<1	//����ͨ��4
#define RX_P_NO_5	5<<1	//����ͨ��5
#define TX_FULL		1<<0	//1: TX FIFO full.


/*---------------------------- λ���� -----------------------------------*/
#define CSN_Low()						NRF_CSN_GPIO_Port->BRR = NRF_CSN_Pin
#define CSN_High()					NRF_CSN_GPIO_Port->BSRR = NRF_CSN_Pin
#define CE_Low()						NRF_CE_GPIO_Port->BRR = NRF_CE_Pin
#define CE_High()						NRF_CE_GPIO_Port->BSRR = NRF_CE_Pin
#define IRQ_isHigh()				(NRF_IRQ_GPIO_Port->IDR)&NRF_IRQ_Pin
#define IRQ_isLow()					!((NRF_IRQ_GPIO_Port->IDR)&NRF_IRQ_Pin)

#define CSN2_Low()					SPI2_CSN_GPIO_Port->BRR = SPI2_CSN_Pin
#define CSN2_High()					SPI2_CSN_GPIO_Port->BSRR = SPI2_CSN_Pin
#define CE2_Low()						SPI2_CE_GPIO_Port->BRR = SPI2_CE_Pin
#define CE2_High()					SPI2_CE_GPIO_Port->BSRR = SPI2_CE_Pin
#define IRQ2_isHigh()				(SPI2_IRQ_GPIO_Port->IDR)&SPI2_IRQ_Pin
#define IRQ2_isLow()			 !((SPI2_IRQ_GPIO_Port->IDR)&SPI2_IRQ_Pin)
/*--------------------------- λ���� ---------------------------------*/

/*-------------------- Task start --------------------*/
#define NRF_RX_EVENT	0
#define NRF_TX_EVENT	0
/*-------------------- Task end --------------------*/



/** @defgroup CONFIG_Define ����ģʽ���üĴ���
  * @{
  */
/* ---------------------------------------------------------------------- */
#define MASK_RX_DR   1<<6		/* 1:����RX_DS�ж�, ���¼�������ᴥ��RX_DS�¼�
															RX: 1> (û��ʹ���Զ�ȷ��)�յ����ݺ󴥷�
																	2> (ʹ���Զ�ȷ�Ϲ���)�յ����ݺ�,У��Ϊ�����ݺ󴥷�
															TX: 1> (ʹ���Զ��ط�����,���ͷ�NO_ACK��),�յ�Ӧ��ACK�󴥷�	*/
/* ---------------------------------------------------------------------- */
#define MASK_TX_DS   1<<5		/* 1:����TX_DS�ж�, ���¼�������ᴥ��TX_DS�¼�
															TX: 1> (û��ʹ���Զ��ط�)�������ݺ�ᴥ��
																	2> (ʹ���Զ��ط�,����NO_ACK��),�������ݺ�ᴥ��
																	3> (ʹ���Զ��ط�,��NO_ACK��),�������ݺ�,�յ�ACKӦ��  
															RX: 1> (ʹ���Զ�ȷ�Ϲ���)�յ����ݺ�,Ӧ��ACK,ACK�и�����Ч�غ�	*/
/* ---------------------------------------------------------------------- */
#define MASK_MAX_RT  1<<4		/* 1:��������ط������ж�    */
/* ---------------------------------------------------------------------- */
#define EN_CRC       1<<3		/* ʹ��CRC(����Ҳʹ���Զ�ȷ�Ϲ���) 1:ʹ��   */
/* ---------------------------------------------------------------------- */
#define CRCO				 1<<2		/* CRC�ֽ��� 0:1byte 1:2byte    */
/* ---------------------------------------------------------------------- */
#define PWR_UP       1<<1		/* 1:POWER UP 0:POWER DOWN    */
/* ---------------------------------------------------------------------- */
#define PRIM_RX      1<<0		/* 1:RX 0:TX    */
/* ---------------------------------------------------------------------- */


/** @defgroup EN_AA_Define ʹ��RX�Զ�ȷ�Ϲ���(Enhanced ShockBurst)
  * @{
  */
#define ENAA_P5			1<<5		/* ʹ��RXͨ��5�Զ�ȷ�� 1:ʹ�� */
#define ENAA_P4			1<<4		/* ʹ��RXͨ��4�Զ�ȷ�� 1:ʹ�� */
#define ENAA_P3			1<<3		/* ʹ��RXͨ��3�Զ�ȷ�� 1:ʹ�� */
#define ENAA_P2			1<<2		/* ʹ��RXͨ��2�Զ�ȷ�� 1:ʹ�� */
#define ENAA_P1			1<<1		/* ʹ��RXͨ��1�Զ�ȷ�� 1:ʹ�� */
#define ENAA_P0			1<<0		/* ʹ��RXͨ��0�Զ�ȷ�� 1:ʹ�� */


/** @defgroup EN_RXADDR_Define ʹ�ܽ���ͨ��
  * @{
  */
#define ERX_P5			1<<5		/* ʹ�ܽ���ͨ��5 1:ʹ�� */
#define ERX_P4			1<<4		/* ʹ�ܽ���ͨ��4 1:ʹ�� */
#define ERX_P3			1<<3		/* ʹ�ܽ���ͨ��3 1:ʹ�� */
#define ERX_P2			1<<2		/* ʹ�ܽ���ͨ��2 1:ʹ�� */
#define ERX_P1			1<<1		/* ʹ�ܽ���ͨ��1 1:ʹ�� */
#define ERX_P0			1<<0		/* ʹ�ܽ���ͨ��0 1:ʹ�� */


/** @defgroup SETUP_AW_Define ���õ�ַ���[3-5]byte
  * @{
  */
#define AW_WIDTH_3_BYTE		1	/* ��ַ���3byte */
#define AW_WIDTH_4_BYTE		2	/* ��ַ���4byte */
#define AW_WIDTH_5_BYTE		3	/* ��ַ���5byte */


/** @defgroup SETUP_RETR_Define �����Զ��ش�
  * @{
  */
#define AUTO_RETRANSMIT_DELAY_US(x) 	(x/250-1)	/* �Զ��ش�ʱ��250us������,���4000us,����500us */
#define AUTO_RETRANSMIT_COUNT(x)			(x)				/* �Զ��ط����� */


/** @defgroup RF_SETUP_Define ��Ƶ���üĴ���
  * @{
  */
/* ---------------------------------------------------------------------- */
#define CONT_WAVE				1<<7/* �����ز�����,����Ĳ�������,�����ź�,�����ڱ���ͨ��ͨ����,һ�㲻�� */
/* ---------------------------------------------------------------------- */
#define RF_DR_1Mbps			0x00/* ��Ƶ���� 1Mbps		xx(0)x(0)xxx */
#define RF_DR_2Mbps			0x08/* ��Ƶ���� 2Mbps		xx(0)x(1)xxx */
#define RF_DR_250kbps		0x20/* ��Ƶ���� 250kbps	xx(1)x(0)xxx */
/* ---------------------------------------------------------------------- */
#define RF_PWR_n18dB		0x00/* ���书��-18dBm	*/
#define RF_PWR_n12dB		0x02/* ���书��-12dBm	*/
#define RF_PWR_n6dB			0x04/* ���书��-6dBm		*/
#define RF_PWR_0dB			0x06/* ���书�� 0dBm		*/
/* ---------------------------------------------------------------------- */


/** @defgroup STATUS_Define �ж�״̬�Ĵ���
  * @{
  */
#define RX_DR						1<<6						/* RX FIFO�յ����ݱ�־,д1��� */
#define TX_DS						1<<5						/* TX FIFO�������ݱ�־,д1��� */
#define MAX_RT					1<<4						/* �ﵽ����ط�������־,д1��� */
#define RX_P_NOx(reg)		((reg&0x0E)>>1) /* (ֻ��)��ʾ����(0-5)��һ���ݹܵ������� */
#define TX_FULL					1<<0						/* (ֻ��)TX FIFO�ǿձ�־,��Ҫ����ջ����ȴ����� */
	

/** @defgroup OBSERVE_TX_Define ����ʧ�ܼ����Ĵ���
  * @{
  */
#define PLOS_CNTx(reg)	((reg&0xF0)>>4) /* (ֻ��)��������ʧ�����ݰ�����,ͨ��д��RF_CH���ü����� */
#define ARC_CNTx(reg)	((reg&0x0F)) 			/* (ֻ��)����һ���ش��Ĵ���,�����ݿ�ʼ�������� */
	

/** @defgroup FIFO_STATUS_Define FIFO״̬�Ĵ���
  * @{
  */
#define TX_REUSE			1<<6 /* TX_REUSE�غ�������SPI����REUSE_TX_PL����,ͨ��SPI����W_TX_PAYLOAD��FLUSH_TX���� */			
#define TX_FULL_2			1<<5 /* TX FIFO�ǿձ�־,��Ҫ��ջ����ȴ����� */
#define TX_EMPTY			1<<4 /* TX FIFO�ձ�־ */
#define RX_FULL				1<<1 /* RX FIFO�ǿձ�־,��Ҫ�������ݻ���ջ��� */
#define RX_EMPTY			1<<0 /* RX FIFO�ձ�־ */


/** @defgroup DYNPD_Define (���ý��ն�)ʹ�ܶ�̬����
  * @{
  */
#define DPL_P5	1<<5 /* ʹ������ͨ��5��̬���ݳ��� */
#define DPL_P4	1<<4 /* ʹ������ͨ��4��̬���ݳ��� */
#define DPL_P3	1<<3 /* ʹ������ͨ��3��̬���ݳ��� */
#define DPL_P2	1<<2 /* ʹ������ͨ��2��̬���ݳ��� */
#define DPL_P1	1<<1 /* ʹ������ͨ��1��̬���ݳ��� */
#define DPL_P0	1<<0 /* ʹ������ͨ��0��̬���ݳ��� */
	
	
/** @defgroup FEATURE_Define (���÷��Ͷ�)ʹ�ܶ�̬����2
  * @{
  */
#define EN_DPL			1<<2 /* ʹ�ܷ��Ͷ����ö�̬���ݳ��ȱ��ĸ�ʽ */
#define EN_ACK_PAYd	1<<1 /* ʹ��TX�˶�RX��Ч�غɵ��Զ�ȷ�� */
#define EN_DYN_ACK	1<<0 /* ʹ�ܸ������ݲ���Ҫ�Զ�ȷ��(NO_ACK) */


/*NRF��ʼ������*/
typedef struct {
	uint8_t CONFIG_;			/*!< @ref CONFIG_Define ����ģʽ���üĴ��� */
	
	uint8_t EN_AA_;				/*!< @ref EN_AA_Define ʹ���Զ�ȷ�Ϲ���(Enhanced ShockBurst) */
	
	uint8_t EN_RXADDR_;		/*!< @ref EN_RXADDR_Define ʹ�ܽ���ͨ��(0~5) */
	
	uint8_t SETUP_AW_;		/*!< @ref SETUP_AW_Define ���õ�ַ���[3-5]byte */
	
	uint8_t SETUP_RETR_;	/*!< @ref SETUP_RETR_Define �����Զ��ش� */
	
	uint8_t	RF_CH_;				/* ��ƵƵ��,��7λ����,[0-127]1����1MHz 2400MHz~2525MHz	*/
	
	uint8_t RF_SETUP_;		/*!< @ref RF_SETUP_Define ��Ƶ���üĴ��� */
	
	uint8_t STATUS_;			/*!< @ref STATUS_Define �ж�״̬�Ĵ��� */ 					/**** �ɶ���д,�����־ ****/
	
	uint8_t OBSERVE_TX_;	/*!< @ref OBSERVE_TX_Define ����ʧ�ܼ����Ĵ��� */ 	/**** ���ɶ� ****/
	
	uint8_t RPD_;					/* �����ڼ���ź�ǿ��	*/													/**** ���ɶ� ****/
	
	uint8_t RX_ADDR_P0_[5];	/* ͨ��0��ַ */
	uint8_t RX_ADDR_P1_[5];	/* ͨ��1��ַ */
	uint8_t RX_ADDR_P2_[5];	/* ͨ��2��ַ */
	uint8_t RX_ADDR_P3_[5];	/* ͨ��3��ַ */
	uint8_t RX_ADDR_P4_[5];	/* ͨ��4��ַ */
	uint8_t RX_ADDR_P5_[5];	/* ͨ��5��ַ */
	
	uint8_t TX_ADDR_[5];	/* ���͵�ַ */
	
	uint8_t	RX_PW_P0_;		/* ͨ��0��̬��Ч�غɳ��� [1-32] (ʹ�ܶ�̬���Ȳ�������) */
	uint8_t	RX_PW_P1_;		/* ͨ��1��̬��Ч�غɳ��� [1-32] (ʹ�ܶ�̬���Ȳ�������) */
	uint8_t	RX_PW_P2_;		/* ͨ��2��̬��Ч�غɳ��� [1-32] (ʹ�ܶ�̬���Ȳ�������) */
	uint8_t	RX_PW_P3_;		/* ͨ��3��̬��Ч�غɳ��� [1-32] (ʹ�ܶ�̬���Ȳ�������) */
	uint8_t	RX_PW_P4_;		/* ͨ��4��̬��Ч�غɳ��� [1-32] (ʹ�ܶ�̬���Ȳ�������) */
	uint8_t	RX_PW_P5_;		/* ͨ��5��̬��Ч�غɳ��� [1-32] (ʹ�ܶ�̬���Ȳ�������) */
	
	uint8_t FIFO_STATUS_;	/*!< @ref FIFO_STATUS_Define FIFO״̬�Ĵ��� */			/**** ���ɶ� ****/
	
	uint8_t DYNPD_;				/*!< @ref DYNPD_Define ʹ�ܶ�̬���� */
	
	uint8_t FEATURE_;			/*!< @ref FEATURE_Define ʹ�ܶ�̬����2 */

} NRF24L01_TypeDef;



uint8_t SPI_RW_Reg( uint8_t reg, uint8_t value, SPI_HandleTypeDef *hspi );//��ȡ�Ĵ���
uint8_t SPI_Write_Buf( uint8_t reg, uint8_t *pBuf, uint8_t len, SPI_HandleTypeDef *hspi );//���ֽ�д��
uint8_t SPI_Read_Buf( uint8_t reg, uint8_t *pBuf, uint8_t len, SPI_HandleTypeDef *hspi );//���ֽڶ�ȡ
uint8_t Nrf24l01_Init( NRF24L01_TypeDef* nrf, SPI_HandleTypeDef *hspi );
void Tx_Mode( SPI_HandleTypeDef *hspi );
void Rx_Mode( SPI_HandleTypeDef *hspi );

void nrf_init(void);
void nrf1_receive_data(void);//��������
void nrf2_send_data(void);

//void NRF_TX_ITRPT(void);
//void CleanStatus(uint8_t temp); // �����־λ
//uint8_t ReadStatus(void);	// ��ȡ��־λ
//void ReadSuccess(void); // �������ݳɹ�����
//void SendMax(void);	// ���ʹ���������
//void SendDatAndSuccess(void);	// �������ݻ���ɹ�
//void NRF2_Init(void);
//void Tx2_Mode( void );


#endif /*__BSP_NRF__H*/


