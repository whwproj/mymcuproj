#include "../Bsp/bsp_sdio.h"

HAL_SD_CardInfoTypeDef SDCardInfo;
DRESULT ret;
unsigned char send_buf[512];
char uart_buf[512];

void printf_sdcard_info(HAL_SD_CardInfoTypeDef SDCardInfo) {
	uint64_t CardCap; //SD卡容量
	HAL_SD_CardCIDTypeDef SDCard_CID;
	HAL_SD_GetCardCID(&hsd,&SDCard_CID); //获取CID
	HAL_SD_GetCardInfo(&hsd,&SDCardInfo); //获取SD卡信息
	CardCap=(uint64_t)(SDCardInfo.LogBlockNbr)*(uint64_t)(SDCardInfo.LogBlockSize); //计算SD卡容量
	switch(SDCardInfo.CardType) {
		case CARD_SDSC:
		{
			if(SDCardInfo.CardVersion == CARD_V1_X)
				printf("Card Type:SDSC V1\r\n");
			else if(SDCardInfo.CardVersion == CARD_V2_X)
				printf("Card Type:SDSC V2\r\n");
		} break;
		case CARD_SDHC_SDXC:printf("Card Type:SDHC\r\n");
			break;
		default:break;
	}
	printf("Card ManufacturerID: %d \r\n",SDCard_CID.ManufacturerID); //制造商ID
	printf("CardVersion: %d \r\n",(uint32_t)(SDCardInfo.CardVersion)); //卡版本号
	printf("Class: %d \r\n",(uint32_t)(SDCardInfo.Class)); //SD卡类别
	printf("Card RCA(RelCardAdd): %d \r\n",SDCardInfo.RelCardAdd); //卡相对地址
	printf("Card BlockNbr: %d \r\n",SDCardInfo.BlockNbr); //块数量
	printf("Card BlockSize: %d \r\n",SDCardInfo.BlockSize); //块大小
	printf("LogBlockNbr: %d \r\n",(uint32_t)(SDCardInfo.LogBlockNbr)); //逻辑块数量
	printf("LogBlockSize: %d \r\n",(uint32_t)(SDCardInfo.LogBlockSize)); //逻辑块大小
	printf("Card Capacity: %d MB\r\n",(uint32_t)(CardCap>>20)); //卡容量
}

void sdio_test_fun( void ) {
	printf("init...\r\n");
	SD_Driver.disk_initialize(0);
	printf_sdcard_info(SDCardInfo);
	printf("\r\n\r\n********** 读写测试 **********\r\n");
	memset( send_buf, 0, 512 );
	memset( uart_buf, 0, 512 );
	ret = SD_Driver.disk_write(0, (BYTE *)"L is too short to spend 阿阿嗄time abc come 的喂\r\n",20,2);
	printf("sd write result:%d\r\n", ret);
	ret = SD_Driver.disk_read(0, send_buf, 20, 2);
	
	printf("sd reak result:%d\r\n", ret);
	if ( strlen( (char*)send_buf ) < 30 ) {
		printf("sd read content:\r\n%s\r\n", send_buf);
	} else {
		memcpy( uart_buf,  send_buf, 100  );
		printf("sd read content:\r\n%s\r\n", uart_buf);
	}
	printf("\r\n\r\n********** end **********\r\n");
}




