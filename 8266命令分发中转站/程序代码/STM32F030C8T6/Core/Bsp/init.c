#include "../common.h"

static void cyclicShift( uint32_t *cpuID ) {
	uint32_t cpuTemp;
	cpuTemp = cpuID[2];
	cpuID[2] = cpuID[1];
	cpuID[1] = cpuID[0];
	cpuID[0] = cpuTemp;
}

//通过唯一设备ID获取SN
void getSnByDeviceId_setClientId( void ) {
	uint32_t cpuID[3];
	//uint8_t crcTemp;
	uint8_t sn[3];
	
	cpuID[0] = *(uint32_t *)UID_BASE;
	cpuID[1] = *(uint32_t *)UID_BASE+4;
	cpuID[2] = *(uint32_t *)UID_BASE+8;
	
	sn[0] = crc8((uint8_t *)cpuID, 12);
	cyclicShift( cpuID );//循环位移
	sn[1] = crc8((uint8_t *)cpuID, 12); 
	cyclicShift( cpuID );//循环位移
	sn[2] = crc8((uint8_t *)cpuID, 12); 
	//sn[3] = crcTemp >> 8;
	//cyclicShift( cpuID );//循环位移								
	//sn[4] = crc8((uint8_t *)cpuID, 12);
	
	//设置clientId
	sprintf( udata.duid, "%.2X%.2X%.2X", sn[0],sn[1],sn[2] );
	//sprintf( pBatt->sn, "DCGV4%.2X%.2X%.1Xxxx", sn[0],sn[1],sn[2] );

	//sprintf( net_str.clientId, "%s@", pBatt->sn );
}

