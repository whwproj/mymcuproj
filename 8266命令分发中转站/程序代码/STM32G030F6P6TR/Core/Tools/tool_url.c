#include "../Tools/tool_url.h"

char* encodeURL( char *url ) {
	return NULL;
}

uint8_t a_f_to_u8( uint8_t a_f ) {
	if ( a_f < '9' ) {
		return a_f-'0';
	} else {//A-65 F-70
		return a_f-55;
	}
}

char* decodeURL( const char * const url ) {
	char *str1, *str2;
	char *newStr, *cpyStr;
	uint8_t len;
	
	newStr = pvPortMalloc( strlen(url) );
	memset( newStr, 0, strlen(url) );
	cpyStr = newStr;
	
	if ( strstr( url, "%" )==NULL ) {
		sprintf( newStr, "%s", url );
		return cpyStr;
	}
	
	printf("url: %s\r\n", url);
	str1 = (char*)url;
	str2 = (char*)url;
	str2 = strstr( str1, "%" );
	
	while( str2 != NULL ) {
		len = (uint8_t)((uint32_t)str2-(uint32_t)str1);
		memcpy( newStr, str1, len );
		newStr += len;
		str2++;
		newStr[0] = a_f_to_u8(str2[0]) * 16 + a_f_to_u8(str2[1]);
		newStr[1] = '\0';
		newStr++;
		str1 = str2 + 2;
		str2 = strstr( str1, "%" );
	}
	printf("str: %s\r\n", cpyStr);
		
}