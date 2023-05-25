#include "../Bsp/bsp_flash.h"

//扇区 0 		0x0800 0000 - 0x0800 07FF 2 KB
//...
//扇区 255 	0x0807 F800 - 0x0807 FFFF 2 KB
#define PARAM_ADDR	0x0800C000

PARAM_STR param_str;

void eraseSector( void ) {
	FLASH_EraseInitTypeDef eraseInitStruc;
	uint32_t SectorError;
	
	eraseInitStruc.Banks = FLASH_BANK_1;
	eraseInitStruc.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInitStruc.PageAddress = PARAM_ADDR;
	eraseInitStruc.NbPages = 1;
	
	__HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_BSY | FLASH_FLAG_PGERR | 
													FLASH_FLAG_WRPERR | FLASH_FLAG_EOP );
	
	HAL_FLASH_Unlock();
	
	HAL_FLASHEx_Erase(&eraseInitStruc, &SectorError);
	
	HAL_FLASH_Lock();
	
}

//static void wFlash4096Byte_Inner( void ) {
//	static uint32_t data, addr;
//	uint16_t i;
//	static uint8_t *pBuff;
//	if ( sd_firmware.nowSize == sd_firmware.size ) {
//		pBuff = sd_firmware.sdBuff;
//	}
//	if ( (sd_firmware.nowSize % 4096 == 0)
//		|| (sd_firmware.nowSize == sd_firmware.size ) ) {
//		pBuff = sd_firmware.sdBuff;
//	}
//	if ( sd_firmware.nowSize == 4096 ) {//首次写入
//		addr = FIRMWARE_START_ADDR;
//	}
//	
//		
//	HAL_FLASH_Unlock();
//	
//	for ( i=0; i<1024; i++ ) {
//		data = pBuff[3]<<24;
//		data |= pBuff[2]<<16;
//		data |= pBuff[1]<<8;
//		data |= pBuff[0];
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data);
//		pBuff += 4;
//		addr += 4;
//	}
//	
//	HAL_FLASH_Lock(); 
//}





