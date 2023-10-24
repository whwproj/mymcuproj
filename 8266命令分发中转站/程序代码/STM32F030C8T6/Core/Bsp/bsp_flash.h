#ifndef __BSP__FLASH__H
#define __BSP__FLASH__H

#include "../common.h"

#define FLASH_ADDR_PAGE0	0x08000000
#define FLASH_ADDR_PAGE1	0x08000800
#define FLASH_ADDR_PAGE2	0x08001000
#define FLASH_ADDR_PAGE3	0x08001800
#define FLASH_ADDR_PAGE4	0x08002000
#define FLASH_ADDR_PAGE5	0x08002800
#define FLASH_ADDR_PAGE6	0x08003000
#define FLASH_ADDR_PAGE7	0x08003800
#define FLASH_ADDR_PAGE8	0x08004000
#define FLASH_ADDR_PAGE9	0x08004800
#define FLASH_ADDR_PAGE10	0x08005000
#define FLASH_ADDR_PAGE11	0x08005800
#define FLASH_ADDR_PAGE12	0x08006000
#define FLASH_ADDR_PAGE13	0x08006800
#define FLASH_ADDR_PAGE14	0x08007000
#define FLASH_ADDR_PAGE15	0x08007800

#define FLASH_NRFADDR_MEM 0x800F800
#define FLASH_NRFADDR			0x800FC00


typedef struct _USER_DATA {
	char wssid[33];//最长32byte
	char wpswd[21];//最长20byte
	char tcpurl[43];//最长42byte
	char mqusername[51];//mqtt username 50byte
	char mqpasswd[33];//mqtt password 32byte
	uint16_t tcpport;
	char duid[9];//mqtt连接时的ClientId
} USER_DATA;

extern USER_DATA udata;

void user_flash_erase( void );//擦除用户flash
void read_data_from_flash( void );//从flash中读取数据
void write_data_into_flash( void );//写入数据到flash

uint32_t get_nrfaddr_by_deviceId( uint8_t id );//根据deviceId查找nrfAddr
int insert_nrfaddr( uint8_t id );//对应deviceId新增/更新nrfAddr
uint32_t create_deviceId( uint8_t id );//生成新的deviceId并存入nrfAddr

#endif /*__BSP__FLASH__H*/



