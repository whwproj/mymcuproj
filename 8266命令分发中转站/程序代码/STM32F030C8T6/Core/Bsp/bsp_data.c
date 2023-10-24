#include "../Bsp/bsp_data.h"

DATA_STR data_str;
ASK_STR ask_str;

//解析wifi数据
UBaseType_t parse_wifi_data_fun( void ) {
	char* jsonStr = pvPortMalloc(100);
	if ( xQueueReceive( wifi_data_handle, jsonStr, pdFALSE ) ) {

		//解析json
		if ( cjson_pase_method( (uint8_t*)jsonStr ) == -1 ) {
			printf("json数据无效\r\n");
			vPortFree(jsonStr);
			return uxQueueMessagesWaiting( wifi_data_handle );
		}
		
		//封装nrfData
		nrf_pack_data( data_str.nrfid, data_str.code, data_str.data );

		//发送nrfData
		xTaskNotify( nrf_control_taskHandle, 1U<<NRF_TX_EVENT, eSetBits );
	}
	vPortFree(jsonStr);
	return uxQueueMessagesWaiting( wifi_data_handle );
}

//添加到计时队列
int add_data_to_list_fun( uint8_t deviceId, uint16_t pcode ) {
	int res = -1;
	while( ask_str.useing ) { osDelay(1); }
	ask_str.useing = 1;
	if ( (ask_str.list & 0x1F) == 0x1F ) {
		goto END;
	}
	for ( int i=0; i<5; i++ ) {
		if ( !( ask_str.list & (1U<<i) ) ) {
			ask_str.list |= 1U<<i;
			ask_str.sendTicks[i] = HAL_GetTick();
			ask_str.deviceIds[i] = deviceId;
			ask_str.pcode[i] = pcode;
			res = 0;
			goto END;
		}
	}
	
	END:
	ask_str.useing = 0;
	return res;
}
	

//发送数据成功后计时
void nrf_send_time_fun( void ) {
	uint8_t i, fastIdx;
	uint32_t /*newBits, */nowTick, fastTick = 0xFFFFFFFF;
	
	for ( ;; ) {
		//获取信号量,访问list锁,useing=1不可访问
		while( ask_str.useing ) { osDelay(1); }
		ask_str.useing = 1;
		
		if ( (ask_str.list & 0x1F) == 0 ) { 
			ask_str.useing = 0;
			break;
		}
		nowTick = HAL_GetTick();
		for ( i=0; i<5; i++ ) {
			if ( ( ask_str.list & (1U<<i) ) ) {
				if ( ask_str.sendTicks[i] <= fastTick ) {
					fastTick = ask_str.sendTicks[i];
					fastIdx = i;
				}
			}
		}
		
		if ( nowTick < fastTick ) {
			if ( 0xFFFFFFFF - fastTick + nowTick + 1 > 10 ) {//超过10ms
				//应答失败
				while ( !w_str.sendLock ) { osDelay(1); }
				w_str.session.deviceId = ask_str.deviceIds[fastIdx];
				w_str.session.code = ask_str.pcode[fastIdx];
				send_device_not_online();
				ask_str.list &=~ (1<<fastIdx);
				ask_str.deviceIds[fastIdx] = 0;
				ask_str.sendTicks[fastIdx] = 0;
				//xTaskNotify( wifi_control_taskHandle, 1U<<DEVICE_NOT_ONLINE, eSetBits );
				
			} else {
				ask_str.useing = 0;
				osDelay( 0xFFFFFFFF - fastTick + nowTick + 1 );
				//xTaskNotifyWait( pdFALSE, pdFALSE, &newBits, (0xFFFFFFFF - fastTick + nowTick + 1) );
			}
		} else {
			if ( nowTick - fastTick > 10 ) {//超过10ms
				//应答失败
				while ( !w_str.sendLock ) { osDelay(1); }
				w_str.session.deviceId = ask_str.deviceIds[fastIdx];
				w_str.session.code = ask_str.pcode[fastIdx];
				send_device_not_online();
				ask_str.list &=~ (1<<fastIdx);
				ask_str.deviceIds[fastIdx] = 0;
				ask_str.sendTicks[fastIdx] = 0;
				xTaskNotify( wifi_control_taskHandle, 1U<<DEVICE_NOT_ONLINE, eSetBits );
			} else {
				ask_str.useing = 0;
				osDelay( nowTick - fastTick );
				//xTaskNotifyWait( pdFALSE, pdFALSE, &newBits, nowTick-fastTick );
			}
		}
		ask_str.useing = 0;
	}
}
	





