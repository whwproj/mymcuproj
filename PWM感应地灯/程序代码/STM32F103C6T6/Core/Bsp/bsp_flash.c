#include "../Bsp/bsp_flash.h"

//ÉÈÇø 0 		0x0800 0000 - 0x0800 07FF 2 KB
//...
//ÉÈÇø 31 	0x0800 7C00 - 0x0800 8400 2 KB
#define PARAM_ADDR	0x08007C00

PARAM_STR param_str;

void write_to_flash( void ) {
	FLASH_EraseInitTypeDef eraseInitStruc;
	uint32_t SectorError, data, addr;
	uint16_t strLen;
	uint8_t *buff, time, yu;
	
	//eraseInitStruc.Banks = FLASH_BANK_1;
	eraseInitStruc.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInitStruc.PageAddress = PARAM_ADDR;
	eraseInitStruc.NbPages = 1;
	
	__HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_BSY | FLASH_FLAG_PGERR | 
													FLASH_FLAG_WRPERR | FLASH_FLAG_EOP );
	
	HAL_FLASH_Unlock();
	
	HAL_FLASHEx_Erase(&eraseInitStruc, &SectorError);
	
	buff = (uint8_t *)&param_str;
	strLen = sizeof( PARAM_STR );
	time = strLen / 4;
	yu = strLen % 4;
	addr = PARAM_ADDR;
	
	for ( ; time>0; time-- ) {
		data = buff[0];
		data |= buff[1]<<8;
		data |= buff[2]<<16;
		data |= buff[3]<<24;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data);
		buff += 4;
		addr += 4;
	}
	if ( yu >= 1 ) { data = buff[0]; }
	if ( yu >= 2 ) { data |= buff[1]<<8; }
	if ( yu == 3 ) { data |= buff[2]<<16; }
	if ( yu > 0 ) { HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data); }
	HAL_FLASH_Lock(); 
	
}

void read_from_flash( void ) {
	memcpy( &param_str, (uint8_t *)PARAM_ADDR, sizeof( PARAM_STR ) );
}




