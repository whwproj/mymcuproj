#include "../common.h"

static void cyclicShift( uint32_t *cpuID ) {
	uint32_t cpuTemp;
	cpuTemp = cpuID[2];
	cpuID[2] = cpuID[1];
	cpuID[1] = cpuID[0];
	cpuID[0] = cpuTemp;
}

//通过唯一设备ID获取SN
//中转站地址: {0xC8,0x8F,0xE6,0x96}
void getSnByDeviceId_setClientId( void ) {
	uint32_t cpuID[3];
	uint8_t sn[4];
	
	uint8_t TS_ADDR[4] = {0xC8,0x8F,0xE6,0x96};
	
	cpuID[0] = *(uint32_t *)UID_BASE;
	cpuID[1] = *(uint32_t *)UID_BASE+4;
	cpuID[2] = *(uint32_t *)UID_BASE+8;
	
	sn[0] = crc8((uint8_t *)cpuID, 12);
	cyclicShift( cpuID );//循环位移
	sn[1] = crc8((uint8_t *)cpuID, 12);
	cyclicShift( cpuID );//循环位移
	sn[2] = crc8((uint8_t *)cpuID, 12);
	sn[3] = crc8((uint8_t *)cpuID, 4);
	//设置clientId
	//sprintf( udata.duid, "%.2X%.2X%.2X", sn[0],sn[1],sn[2] );
	
	//设置NRF地址
	memset( &nrf_str, 0, sizeof(NRF_STR) );
	memcpy( nrf_str.localAddr, sn, 4 );
	memcpy( nrf_str.TSAddr, TS_ADDR, 4 );
}


