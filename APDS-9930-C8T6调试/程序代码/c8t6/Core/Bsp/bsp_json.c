#include "../Bsp/bsp_json.h"

static char* cjson_reply_template( uint32_t pvcode, uint8_t sta, char* msg );


char* cjson_pase_method( uint8_t *pdBuff ) {
	cJSON *trunk = NULL;
	cJSON *code = NULL;
	cJSON *nrfid = NULL;
	cJSON *data = NULL;
	char *daStr, *jsonStr;
	uint32_t pvCode;
	uint8_t nrfid_v;
	
	trunk = cJSON_Parse((char *)pdBuff);
	if (trunk == NULL) return NULL;

	code = cJSON_GetObjectItemCaseSensitive(trunk, "code");
	if ( !cJSON_IsNumber(code) || (code->valueint == 0) ) {
		cJSON_Delete( trunk );
		return NULL;
	}
	pvCode = code->valueint;
	
	nrfid = cJSON_GetObjectItemCaseSensitive(trunk, "nrfid");
	if ( !cJSON_IsNumber(nrfid) || (nrfid->valueint == 0) ) {
		cJSON_Delete( trunk );
		return NULL;
	}
	nrfid_v = nrfid->valueint;
	
	data = cJSON_GetObjectItemCaseSensitive(trunk, "data");
	if ( !cJSON_IsString(data) || (data->valuestring == NULL) ) {
		cJSON_Delete( trunk );
		return NULL;
	}
	daStr = pvPortMalloc( strlen(data->valuestring) );
	sprintf( daStr, "%s", data->valuestring );
	//printf("\r\ndaStr:%s\r\n", daStr);
	vPortFree( daStr );
	
	cJSON_Delete( trunk );

	//Test
	jsonStr = cjson_reply_template( pvCode, 1, "sucess" );
	//printf("\r\nTest:%s\r\n", jsonStr);
	return jsonStr;
}


static char* cjson_reply_template( uint32_t pvcode, uint8_t sta, char* msg ) {
	/*cJSON *trunk = NULL;
	char *string;
	trunk = cJSON_CreateObject();
	cJSON_AddItemToObject( trunk, "pvcode", cJSON_CreateNumber( pvcode ) );
	cJSON_AddItemToObject( trunk, "sta", cJSON_CreateBool( sta ) );
	cJSON_AddItemToObject( trunk, "msg", cJSON_CreateString( msg ) );
	string = cJSON_Print( trunk );
	cJSON_Delete( trunk );
	return string;*/
	char *string = pvPortMalloc(128);
	char *str_t = pvPortMalloc(6);
	if ( sta == 0 ) {
		sprintf(str_t, "false");
	} else {
		sprintf(str_t, "true");
	}
	sprintf( string, "%s%d%s%s%s%s%s",
								PRINT_STR_0, pvcode, 
								PRINT_STR_1, str_t, 
								PRINT_STR_2, msg,
								PRINT_STR_END);
	vPortFree( str_t );
	return string;
}


