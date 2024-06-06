#include "../Tools/http.h"

#define HTTP_PATH	ota_str.path
#define HTTP_HOST	ota_str.host
#define HTTP_PORT	ota_str.port

//	GET /t.c HTTP/1.1
//	User-Agent: Mozilla/5.0
//	Cache-Control: no-cache
//	Accept: */*
//	Connection: keep-alive
//	Range: bytes=0-128
//	Host: 10.0.2.88
int pack_http_con( char *buff, uint32_t startByte, uint32_t endByte) {
	uint8_t num;
	int len = 0;
	num = sprintf(buff, "GET %s HTTP/1.1\r\n", HTTP_PATH);
	if ( num > 0 ) { buff += num; len += num; }
	else return -1;
	
	num = sprintf(buff, "User-Agent: Mozilla/5.0\r\n");
	if ( num > 0 ) { buff += num; len += num; }
	else return -1;
	
	num = sprintf(buff, "Cache-Control: no-cache\r\n");
	if ( num > 0 ) { buff += num; len += num; }
	else return -1;
	
	num = sprintf(buff, "Accept: */*\r\n");
	if ( num > 0 ) { buff += num; len += num; }
	else return -1;
	
	num = sprintf(buff, "Connection: keep-alive\r\n");
	if ( num > 0 ) { buff += num; len += num; }
	else return -1;
	
	num = sprintf(buff, "Range: bytes=%d-%d\r\n", startByte, endByte);
	if ( num > 0 ) { buff += num; len += num; }
	else return -1;
	
	num = sprintf(buff, "Host: %s:%d\r\n\r\n", HTTP_HOST, HTTP_PORT);
	if ( num > 0 ) { len += num; }
	else return -1;
	
	return len;
}

//HTTP/1.1 206 Partial Content
//Server: nginx
//Date: Mon, 12 Jun 2023 08:48:19 GMT
//Content-Type: application/octet-stream
//Content-Length: 129
//Last-Modified: Mon, 12 Jun 2023 07:48:03 GMT
//Connection: keep-alive
//ETag: "6486cdb3-3db8"
//Content-Range: bytes 128-256/15800
//xxxxxxxxxxxxxxxxxxxxxxx
	
















