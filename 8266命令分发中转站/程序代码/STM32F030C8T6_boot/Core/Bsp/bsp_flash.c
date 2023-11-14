#include "../Bsp/bsp_flash.h"

USER_DATA udata;

//擦除用户flash
void user_flash_erase( void ) {
//	FLASH_EraseInitTypeDef eraseInitStruc;
//	uint32_t SectorError;
//	
//	eraseInitStruc.Banks = FLASH_BANK_1;
//	eraseInitStruc.TypeErase = FLASH_TYPEERASE_PAGES;
//	eraseInitStruc.Page = 15;
//	eraseInitStruc.NbPages = 1;
//	
//	__HAL_FLASH_CLEAR_FLAG(
//		FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
//	FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
//	
//	HAL_FLASH_Unlock();
//	
//	HAL_FLASHEx_Erase(&eraseInitStruc, &SectorError);
//	
//	HAL_FLASH_Lock();
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

//FLASH_BANK1 	0x8000000 - 0x80003FF
//FLASH_BANK2 	0x8000400 - 0x80007FF
//FLASH_BANK63 	0x800F800 - 0x800FBFF
//FLASH_BANK64 	0x800FC00 - 0x800FFFF
//存储NRF设备地址的FLASH选用 FLASH_BANK63 和 FLASH_BANK64
//为节省内存,FLASH_BANK63 做为 FLASH_BANK64 读取的缓存
//根据deviceId查找nrfAddr:
uint32_t get_nrfaddr_by_deviceId( uint8_t id ) {
	uint32_t *flash_add, nrf_addr;
	uint16_t offset;
	
	offset = id * 4;//存储方式: xxxx xxxx xxxx
	flash_add = (uint32_t *)(FLASH_NRFADDR + offset);
	nrf_addr = (uint32_t)(* flash_add);
	if ( nrf_addr==0 || nrf_addr==0xFFFFFFFF ) {
		return 0;//不存在id
	}
	nrf_str.deviceAddr[0] = (uint8_t)(nrf_addr);
	nrf_str.deviceAddr[1] = (uint8_t)(nrf_addr>>8);
	nrf_str.deviceAddr[2] = (uint8_t)(nrf_addr>>16);
	nrf_str.deviceAddr[3] = (uint8_t)(nrf_addr>>24);
	return nrf_addr;
}

//生成新的deviceId并存入nrfAddr
uint32_t create_deviceId( uint8_t id ) {
	uint32_t *flash_add, nrf_addr;
	uint16_t offset;
	uint8_t i;
	//新设备首次注册,分配ID
	for ( i=1; i<255; i++ ) {
		offset = i * 4;
		flash_add = (uint32_t *)(FLASH_NRFADDR + offset);
		nrf_addr = (uint32_t)(* flash_add);
		if ( nrf_addr==0 || nrf_addr==0xFFFFFFFF ) {
			insert_nrfaddr(i);//新增nrfAddr
			break;
		}
	}
	return i;
}

//对应deviceId新增/更新nrfAddr
int insert_nrfaddr( uint8_t id ) {
	uint32_t SectorError, flash_addr_mem, flash_addr, nrf_addr;
	FLASH_EraseInitTypeDef eraseType;
	
	nrf_addr = (uint32_t)(nrf_str.rxBuf[7]<<24 | nrf_str.rxBuf[6]<<16 |
												nrf_str.rxBuf[5]<<8  | nrf_str.rxBuf[4]);
	HAL_FLASH_Unlock();
	
	//1.擦除 FLASH_NRFADDR_MEM, 将 FLASH_NRFADDR 复制到 FLASH_NRFADDR_MEM
	eraseType.PageAddress = FLASH_NRFADDR_MEM;
	eraseType.NbPages = 1;
	eraseType.TypeErase = FLASH_TYPEERASE_PAGES;
	__HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_EOP | FLASH_FLAG_WRPERR);
	HAL_FLASHEx_Erase(&eraseType, &SectorError);
	
	flash_addr_mem = FLASH_NRFADDR_MEM;
	flash_addr = FLASH_NRFADDR;
	for ( uint16_t i=0; i<256; i++ ) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_addr_mem, *((uint32_t *)flash_addr));
		flash_addr_mem += 4;
		flash_addr += 4;
	}
	
	//2.擦除 FLASH_NRFADDR, 将 FLASH_NRFADDR_MEM 分段读取处理新增nrfAddr后存入 FLASH_NRFADDR
	eraseType.PageAddress = FLASH_NRFADDR;
	__HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_EOP | FLASH_FLAG_WRPERR);
	HAL_FLASHEx_Erase(&eraseType, &SectorError);
	
	flash_addr_mem = FLASH_NRFADDR_MEM;
	flash_addr = FLASH_NRFADDR;
	for ( uint16_t i=0; i<256; i++ ) {
		if ( i == id ) {
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_addr, nrf_addr);
			flash_addr += 4;
			flash_addr_mem += 4;
		} else {
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_addr, *((uint32_t *)flash_addr_mem));
			flash_addr += 4;
			flash_addr_mem += 4;
		}
	}
	
	HAL_FLASH_Lock();
	
	return 0;
}



