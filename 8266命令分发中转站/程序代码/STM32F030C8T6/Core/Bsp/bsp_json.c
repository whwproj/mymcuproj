#include "../Bsp/bsp_json.h"


//解析JSON数据,解析结果存入结构体 DATA_STR
int cjson_pase_method( uint8_t *pdBuff ) {
	cJSON *trunk = NULL;
	cJSON *code = NULL;
	cJSON *nrfid = NULL;

	cJSON *data = NULL;
	trunk = cJSON_Parse((char *)pdBuff);
	if (trunk == NULL) return -1;

	code = cJSON_GetObjectItemCaseSensitive(trunk, "code");
	if ( !cJSON_IsNumber(code) || (code->valueint == 0) ) {
		cJSON_Delete( trunk );
		return -1;
	}
	nrf_str.session.code = nrfid->valueint;
	
	nrfid = cJSON_GetObjectItemCaseSensitive(trunk, "nrfid");
	if ( !cJSON_IsNumber(nrfid) || (nrfid->valueint == 0) ) {
		cJSON_Delete( trunk );
		return -1;
	}
	nrf_str.session.deviceId = nrfid->valueint;
	
	data = cJSON_GetObjectItemCaseSensitive(trunk, "data");
	if ( !cJSON_IsString(data) || (data->valuestring == NULL) ) {
		cJSON_Delete( trunk );
		return -1;
	}
	if ( nrf_str.session.data == NULL ) nrf_str.session.data = pvPortMalloc( strlen(data->valuestring) );
	sprintf( nrf_str.session.data, "%s", data->valuestring );
	
	cJSON_Delete( trunk );
	return 0;
}

//错误码errCode: -2:设备未注册 -1:设备离线 0:无 1:执行错误
char* cjson_reply_template( uint16_t pvcode, int errCode, char* msg ) {
	/*cJSON *trunk = NULL;
	char *string;
	trunk = cJSON_CreateObject();
	cJSON_AddItemToObject( trunk, "pvcode", cJSON_CreateNumber( pvcode ) );
	cJSON_AddItemToObject( trunk, "sta", cJSON_CreateBool( sta ) );
	cJSON_AddItemToObject( trunk, "msg", cJSON_CreateString( msg ) );
	string = cJSON_Print( trunk );
	cJSON_Delete( trunk );
	return string;*/
	char *string = pvPortMalloc(100);
	//char *str_t = pvPortMalloc(6);
	sprintf( string, "%s%d%s%d%s%s%s",
								PRINT_STR_0, pvcode, 
								PRINT_STR_1, errCode, 
								PRINT_STR_2, msg,
								PRINT_STR_END);
	//vPortFree( str_t );
	return string;
}


char* cjson_send_data_template( uint16_t pvcode, char* data, uint8_t deviceId ) {
	/*cJSON *trunk = NULL;
	char *string;
	trunk = cJSON_CreateObject();
	cJSON_AddItemToObject( trunk, "pvcode", cJSON_CreateNumber( pvcode ) );
	cJSON_AddItemToObject( trunk, "sta", cJSON_CreateBool( sta ) );
	cJSON_AddItemToObject( trunk, "msg", cJSON_CreateString( msg ) );
	string = cJSON_Print( trunk );
	cJSON_Delete( trunk );
	return string;*/
	char *string = pvPortMalloc(100);
	char *str_t = pvPortMalloc(6);
	sprintf( string, "%s%d%s%d%s%s%s",
								PRINT_STR_0, pvcode, 
								PRINT_STR_3, deviceId,
								PRINT_STR_4, data,  
								PRINT_STR_END);
	vPortFree( str_t );
	return string;
}

