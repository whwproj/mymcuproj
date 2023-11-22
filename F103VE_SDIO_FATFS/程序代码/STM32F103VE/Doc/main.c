#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned		   char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;

#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_PAD '='
#define BASE64DE_FIRST '+'
#define BASE64DE_LAST 'z'

static const char base64en[];
unsigned int base64_encode(const unsigned char *in, unsigned int inlen, char *out);
unsigned int getFileSize(const char* filepath);
void getFileName(char* path, int choice, char* pure_file_name, char* file_name, char* extension_name );


int main( void ) {
	FILE *originFile = NULL;
	FILE *targetFile = NULL;
	uint32_t len, b64Len;
	int i = 0;
    char *buf, *b64Buf;
	char path[300];
    char targetPath[300];
	char pure_file_name[100],file_name[100],extension_name[100];
    memset(pure_file_name, 0, 100);
    memset(file_name, 0, 100);
    memset(extension_name, 0, 100);

	printf("drag...\r\n");
	scanf("%s",path);

	len = getFileSize(path);
    printf("len:%d\r\n", len);
    sprintf( targetPath, "%s.base64.txt", path );
    printf("targetPath: %s\r\n", targetPath);
	originFile = fopen(path, "rb");
	targetFile = fopen(targetPath, "wb");
    buf = malloc(len);

    for ( int i=0; i<len; i++ ) {
        sprintf(&buf[i], "%c", fgetc(originFile));
    }
    
    b64Len = BASE64_ENCODE_OUT_SIZE(len);
    b64Buf = (char*)malloc(b64Len);
    base64_encode(buf, len, b64Buf);

    fwrite( b64Buf, 1, len, targetFile );

	fclose(originFile);
	fclose(targetFile);

}



/* BASE 64 encode table */
static const char base64en[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/',
};
unsigned int base64_encode(const unsigned char *in, unsigned int inlen, char *out) {
	int s;
	unsigned int i;
	unsigned int j;
	unsigned char c;
	unsigned char l;

	s = 0;
	l = 0;
	for (i = j = 0; i < inlen; i++) {
		c = in[i];

		switch (s) {
		case 0:
			s = 1;
			out[j++] = base64en[(c >> 2) & 0x3F];
			break;
		case 1:
			s = 2;
			out[j++] = base64en[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
			break;
		case 2:
			s = 0;
			out[j++] = base64en[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
			out[j++] = base64en[c & 0x3F];
			break;
		}
		l = c;
	}

	switch (s) {
	case 1:
		out[j++] = base64en[(l & 0x3) << 4];
		out[j++] = BASE64_PAD;
		out[j++] = BASE64_PAD;
		break;
	case 2:
		out[j++] = base64en[(l & 0xF) << 2];
		out[j++] = BASE64_PAD;
		break;
	}

	out[j] = 0;

	return j;
}
unsigned int getFileSize(const char* filepath) {
    static unsigned int file_size = 0;

    FILE* fp_r = fopen(filepath, "r");
    if(NULL == fp_r)
    {
        printf("error :fail to open %s\n", filepath);
        return file_size = 0;
    }

    // get file size by move pointer, unit byte
    fseek(fp_r, 0, SEEK_SET);
    fseek(fp_r, 0, SEEK_END);
    file_size = ftell(fp_r);
    fseek(fp_r, 0, SEEK_SET);

    fclose(fp_r);

    return file_size;
}
void getFileName(char* path, int choice, char* pure_file_name, char* file_name, char* extension_name ) {
	//原文链接：https://blog.csdn.net/Louise_Trender/article/details/124636973
    int i,j,flag = 0;
    int len = strlen(path); //取字符串长度
    //获取后缀名 //
    for(i=len-1; i>=0; i--)
        if(path[i] == '.'){  //找到最后一个点号 
            strcpy(extension_name,&path[i+1]);//获取后缀名 
            break;
        } 
    //获取文件名(不含路径,含后缀)       
    for(i=len-1; i>=0; i--)
        if(path[i] == '\\'){ //找到最后一个斜杠
            strcpy(file_name,&path[i+1]);//获取文件名(不含路径,含后缀)
            break;    
        }    
    //获取纯文件名(不含路径和后缀)
    for(i=len-1; i>=0; i--){ 
        if(path[i] == '.' && !flag){ 
            j = i; //记录最后一个点号位置 
            flag = 1;    
        }    
        if(path[i] == '\\'){ //找到最后一个斜杠
            strncpy(pure_file_name,path+i+1,j-i-1);//获取纯文件名(不含路径和后缀)
            break;    
        }    
    }    
    //printf("___extension_name:%s\n",extension_name);
    //printf("___file_name:%s\n",file_name);
    //printf("___pure_file_name:%s\n",pure_file_name);
    // switch(choice){
    //     case 1:
    //         return 0;//file_name;//文件名(不含路径,含后缀)
    //     case 2:
    //         return 0;//pure_file_name;//纯文件名(不含路径和后缀)
    //     case 3:
    //         return 0;//extension_name;//后缀 
    //     default:
    //         return -1;//"choice error!";
    // }
}

#if 0
len--;
	fprintf(pTxt, "uint8_t %s = {\n	", "binArr");//打印变量字符串总是乱符
	while( len-- ) {
		fprintf(pTxt, "0x%02X,", fgetc(pBin));
		if ( i++ >= 15 ) {
			fprintf(pTxt, "\n	");
			i = 0;
		}
		
	}
	fprintf(pTxt, "0x%02X};", fgetc(pBin));
#endif











