#include "battery_info.h"
#include "string.h"
BATT batt;

//uchar const battCode[BATT_SIZE] = {


//};


void battParamInit( void ) {
	unsigned int i,j;
	//i = sizeof(batt.cLayer) / sizeof(chargeLayer);
	strcpy(batt.sn, "SN0042531451");
	
		strcpy(batt.cLayer[0].version.hw, "UUAW_V1.0.0");
		strcpy(batt.cLayer[1].version.hw, "UUAW_V1.0.0");
		strcpy(batt.cLayer[2].version.hw, "UUAW_V1.0.0");
		strcpy(batt.cLayer[3].version.hw, "UUAW_V1.0.0");
		strcpy(batt.cLayer[4].version.hw, "UUAW_V2.0.0");
		strcpy(batt.cLayer[5].version.hw, "UUAW_V2.0.0");
		strcpy(batt.version.hw, "UUAW_V2.0.0");
		
		strcpy(batt.cLayer[0].version.boot, "V1.0.0");
		strcpy(batt.cLayer[1].version.boot, "V1.0.0");
		strcpy(batt.cLayer[2].version.boot, "V1.0.0");
		strcpy(batt.cLayer[3].version.boot, "V1.0.0");
		strcpy(batt.cLayer[4].version.boot, "V2.0.0");
		strcpy(batt.cLayer[5].version.boot, "V2.0.0");
		strcpy(batt.version.boot, "V2.0.0");
		
		strcpy(batt.cLayer[0].version.app, "V1.0.0");
		strcpy(batt.cLayer[1].version.app, "V1.0.0");
		strcpy(batt.cLayer[2].version.app, "V1.0.0");
		strcpy(batt.cLayer[3].version.app, "V1.0.0");
		strcpy(batt.cLayer[4].version.app, "V2.0.0");
		strcpy(batt.cLayer[5].version.app, "V2.0.0");
		strcpy(batt.version.app, "V2.0.0");
		
		strcpy(batt.cLayer[0].version.lastapp, "V1.0.0");
		strcpy(batt.cLayer[1].version.lastapp, "V1.0.0");
		strcpy(batt.cLayer[2].version.lastapp, "V1.0.0");
		strcpy(batt.cLayer[3].version.lastapp, "V1.0.0");
		strcpy(batt.cLayer[4].version.lastapp, "V2.0.0");
		strcpy(batt.cLayer[5].version.lastapp, "V2.0.0");
		strcpy(batt.version.lastapp, "V2.0.0");
		
		batt.cLayer[0].syncChargeNum = 4;
		batt.cLayer[1].syncChargeNum = 6;
		batt.cLayer[2].syncChargeNum = 6;
		batt.cLayer[3].syncChargeNum = 12;
		batt.cLayer[4].syncChargeNum = 12;
		batt.cLayer[5].syncChargeNum = 10;
		
	for( i=0; i<6; i++) {
		batt.cLayer[i].chargeMode = 1;
		
		for( j=0; j<12; j++ ) {
			//strcpy(batt.cLayer[i].cbInfo[j].sn, "UFI120120033130");
			batt.cLayer[i].cbInfo[j].power = 50;
			batt.cLayer[i].cbInfo[j].temperature = 25.2f;
			batt.cLayer[i].cbInfo[j].cycles = 300;
			batt.cLayer[i].cbInfo[j].capacity = 4500;
			batt.cLayer[i].cbInfo[j].batteryVoltage = 40.0f;
			batt.cLayer[i].cbInfo[j].cellVoltage = 40.5f;
			//strcpy(batt.cLayer[i].cbInfo[j].imgurl, "http://www.abcdef.com/img001");
		}
	}
	
}













