#include "../Bsp/bsp_flash.h"

USER_DATA udata;

//擦除用户flash
void user_flash_erase( void ) {
	FLASH_EraseInitTypeDef eraseInitStruc;
	uint32_t SectorError;
	
	eraseInitStruc.Banks = FLASH_BANK_1;
	eraseInitStruc.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInitStruc.Page = 15;
	eraseInitStruc.NbPages = 1;
	
	__HAL_FLASH_CLEAR_FLAG(
		FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	
	HAL_FLASH_Unlock();
	
	HAL_FLASHEx_Erase(&eraseInitStruc, &SectorError);
	
	HAL_FLASH_Lock();
}

//从flash中读取数据
void read_data_from_flash( void ) {
	uint8_t i, j, *pudata;
	uint16_t len_t;
	uint32_t addr, da_t;
	
	addr = FLASH_ADDR_PAGE15;
	pudata = (uint8_t*)&udata;
	len_t = sizeof( USER_DATA );
	i = len_t / 4;
	j = len_t % 4;
	for( ; i>0; i-- ) {
		da_t = *(uint32_t *)addr;
		pudata[0] = da_t>>24;
		pudata[1] = da_t>>16;
		pudata[2] = da_t>>8;
		pudata[3] = da_t>>0;
		addr += 4;
		pudata += 4;
	}
	if ( j > 0 ) {
		da_t = *(uint32_t *)addr;
		if ( j >= 1 ) { pudata[0] = da_t>>24; }
		if ( j >= 2 ) { pudata[1] = da_t>>16; }
		if ( j == 3 ) { pudata[2] = da_t>>8; }
	}
}


//写入数据到flash
void write_data_into_flash( void ) {
	uint8_t i, j, *pudata;
	uint16_t len_t;
	uint32_t addr;
	uint64_t da_t;
	
	addr = FLASH_ADDR_PAGE15;
	pudata = (uint8_t*)&udata;
	len_t = sizeof( USER_DATA );
	user_flash_erase();
	HAL_FLASH_Unlock();
	i = len_t / 8;
	j = len_t % 8;
	// 原数据:0x00112233 0x44556677
	// 变换:  0x44556677 0x00112233
	// 存入后:0x00112233 0x44556677
	for( ; i>0; i-- ) {
		da_t = (uint64_t)pudata[0]<<24;
		da_t |= (uint64_t)pudata[1]<<16;
		da_t |= (uint64_t)pudata[2]<<8;
		da_t |= (uint64_t)pudata[3]<<0;
		da_t |= (uint64_t)pudata[4]<<56;
		da_t |= (uint64_t)pudata[5]<<48;
		da_t |= (uint64_t)pudata[6]<<40;
		da_t |= (uint64_t)pudata[7]<<32;
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, da_t);
		addr += 8;
		pudata += 8;
	}
	if ( j > 0 ) {
		if ( j >= 1 ) { da_t = (uint64_t)pudata[0]<<24; }
		if ( j >= 2 ) { da_t |= (uint64_t)pudata[1]<<16; }
		if ( j >= 3 ) { da_t |= (uint64_t)pudata[2]<<8; }
		if ( j >= 4 ) { da_t |= (uint64_t)pudata[3]<<0; }
		if ( j >= 5 ) { da_t |= (uint64_t)pudata[4]<<56; }
		if ( j >= 6 ) { da_t |= (uint64_t)pudata[5]<<48; }
		if ( j == 7 ) { da_t |= (uint64_t)pudata[6]<<32; }
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, da_t);
	}
	HAL_FLASH_Lock(); 
}






