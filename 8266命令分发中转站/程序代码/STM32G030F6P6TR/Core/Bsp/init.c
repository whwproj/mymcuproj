#include "../common.h"

#if 0
static void cyclicShift( uint32_t *cpuID ) {
	uint32_t cpuTemp;
	cpuTemp = cpuID[2];
	cpuID[2] = cpuID[1];
	cpuID[1] = cpuID[0];
	cpuID[0] = cpuTemp;
}
//测试:
//通过唯一设备ID获取SN
static void getSnByDeviceId_setClientId( void ) {
	uint32_t cpuID[3];
	uint16_t crcTemp;
	uint8_t sn[5];
	
	cpuID[0] = *(uint32_t *)0x1FFF7A10;
	cpuID[1] = *(uint32_t *)0x1FFF7A14;
	cpuID[2] = *(uint32_t *)0x1FFF7A18;
	
	crcTemp = crc16((uint8_t *)cpuID, 12);
	sn[0] = crcTemp;
	sn[1] = crcTemp >> 8;
	cyclicShift( cpuID );//循环位移
	crcTemp = crc16((uint8_t *)cpuID, 12); 
	sn[2] = crcTemp;
	//sn[3] = crcTemp >> 8;
	//cyclicShift( cpuID );//循环位移								
	//sn[4] = crc8((uint8_t *)cpuID, 12);
	
	//sprintf( pBatt->sn, "DCGV4%.2X%.2X%.2X%.2X%.1X", sn[0],sn[1],sn[2],sn[3],sn[4]&0x0F );
	//sprintf( pBatt->sn, "DCGV4%.2X%.2X%.1Xxxx", sn[0],sn[1],sn[2] );
	
	//设置clientId
	//sprintf( net_str.clientId, "%s@", pBatt->sn );

}
#endif