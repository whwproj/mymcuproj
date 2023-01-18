#ifndef __BSP_QSPI__H
#define __BSP_QSPI__H

#include "main.h"

//-----------------------------------------------------
#define RED    0xF800         //定义颜色常量 
#define GREEN  0x07E0
#define BLUE   0x001F
#define WHITE  0xFFFF
#define BLACK  0x0000
#define GRAY   0xEF5D         //0x2410
#define GRAY75 0x39E7 
#define GRAY50 0x7BEF    
#define GRAY25 0xADB5   
//-----------------------------------------------------

//#define QSPI_RS_Clr()   HAL_GPIO_WritePin(xxxx, xxxx, GPIO_PIN_RESET)
//#define QSPI_RS_Set()   HAL_GPIO_WritePin(xxxx, xxxx, GPIO_PIN_SET)

#define QSPI_DA0_Clr() HAL_GPIO_WritePin(SDA0_GPIO_Port, SDA0_Pin, GPIO_PIN_RESET)
#define QSPI_DA0_Set() HAL_GPIO_WritePin(SDA0_GPIO_Port, SDA0_Pin, GPIO_PIN_SET)
#define QSPI_DA1_Clr() HAL_GPIO_WritePin(SDA1_GPIO_Port, SDA1_Pin, GPIO_PIN_RESET)
#define QSPI_DA1_Set() HAL_GPIO_WritePin(SDA1_GPIO_Port, SDA1_Pin, GPIO_PIN_SET)
#define QSPI_DA2_Clr() HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_RESET)
#define QSPI_DA2_Set() HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_SET)
#define QSPI_DA3_Clr() HAL_GPIO_WritePin(SDA3_GPIO_Port, SDA3_Pin, GPIO_PIN_RESET)
#define QSPI_DA3_Set() HAL_GPIO_WritePin(SDA3_GPIO_Port, SDA3_Pin, GPIO_PIN_SET)

#define QSPI_CLK_Clr() HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET)
#define QSPI_CLK_Set() HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET)

#define QSPI_CS_Clr()  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET)
#define QSPI_CS_Set()  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET)

#define QSPI_RES_Clr()  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET)
#define QSPI_RES_Set()  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET)

#define QSPI_BLK_Clr() HAL_GPIO_WritePin(LEDK_GPIO_Port, LEDK_Pin, GPIO_PIN_RESET)
#define QSPI_BLK_Set() HAL_GPIO_WritePin(LEDK_GPIO_Port, LEDK_Pin, GPIO_PIN_SET)

#define QSPI_PWR_EN_Clr()  HAL_GPIO_WritePin(IOVCC_GPIO_Port, IOVCC_Pin, GPIO_PIN_RESET)
#define QSPI_PWR_EN_Set()  HAL_GPIO_WritePin(IOVCC_GPIO_Port, IOVCC_Pin, GPIO_PIN_SET)



typedef enum{
      SINGLE_LINE,
      QUAD_LINE
}ENUM_QSPI_IO_NUM;


void Nv3030BInit(void);
void DispColor(unsigned int color);
void Init_LCD(void);
void BlockWrite(uint16_t Xstart,uint16_t Xend,uint16_t Ystart,uint16_t Yend);
void DispColor(unsigned int color);
	
#endif


