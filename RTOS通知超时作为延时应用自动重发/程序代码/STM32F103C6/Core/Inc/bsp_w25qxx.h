#ifndef __BSP_W25Q64__H
#define __BSP_W25Q64__H

#include "main.h" 
#include "gpio.h"
 
#define SPI_CS_GPIO_Port 	GPIOA
#define SPI_CS_Pin				GPIO_PIN_4

//#define  JedecDeviceID              0xEF3015   //W25X16
//#define  JedecDeviceID              0xEF4015	 //W25Q16
//#define  JedecDeviceID              0XEF4018   //W25Q128
//#define  JedecDeviceID              0XEF4017    //W25Q64
#define  JedecDeviceID              0xEF4016    //W25Q32

/*命令定义-开头*******************************/
#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		    0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	    0xAB 
#define W25X_DeviceID			        0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag                  0x01
#define Dummy_Byte                0xFF
/*命令定义-结尾*******************************/


uint32_t W25Qx_ReadDeviceID(void);//读取DeviceID
uint32_t W25Qx_ReadJedecDeviceID(void);//读取JedecDeviceID
void Erase_W25Qx_Chip(void);//擦除整块芯片
void Erase_Write_data_Sector(uint32_t Address,uint32_t Write_data_NUM);//擦除需要写入数据空间的块
void W25Qx_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//多页写入数据
void W25Qx_Read_data(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);//读取多页数据

void W25Qx_PowerDown(void);//掉电模式
void W25Qx_WAKEUP(void);//唤醒

#endif /*__W25Q64__H*/
