#ifndef __BSP_W25Q64__H
#define __BSP_W25Q64__H

#include "../common.h"
 
#define SPI_CS_GPIO_Port 	GPIOA
#define SPI_CS_Pin				GPIO_PIN_2

#define  JedecDeviceID              0xEF3014   //W25X80
//#define  JedecDeviceID              0xEF4015	 //W25Q16
//#define  JedecDeviceID              0XEF4018   //W25Q128
//#define  JedecDeviceID              0XEF4017    //W25Q64
//#define  JedecDeviceID              0xEF4016    //W25Q32


//外部flash存储地址
#define STR_ADDR1			0x000000//0x000000~0x000FFF 4KB	
#define STR_ADDR2			0x001000//0x001000~0x001FFF 4KB	
#define STR_ADDR3			0x002000//0x002000~0x002FFF 4KB	
#define STR_ADDR4			0x003000//0x003000~0x003FFF 4KB
#define STR_ADDR5			0x004000//0x004000~0x004FFF 4KB
#define STR_ADDREND		0x020000//0x020000~0x11FFFF 1MB


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

void read_data_from_w25qFlash( void );
void write_data_to_w25qFlash( void );
#endif /*__W25Q64__H*/
