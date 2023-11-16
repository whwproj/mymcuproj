#include "../Bsp/bsp_fatfs.h"

BYTE write_buf2[] = "**追加的消息aaaaaaabcd**";
BYTE read_buf[128] = {0};
BYTE temp_buf[128];
BYTE write_buf[128];

#define USERPath "0:/"

void fatfs_init_fun( void ) {
	#define USERPath "0:/"
	FRESULT ret;
	//挂载文件系统，挂载的时候会完成对应硬件设备（SD卡/SDnand）初始化
	ret = f_mount(&SDFatFS, USERPath, 1);
	if (ret != FR_OK) {
		printf("f_mount error!\r\n");
		f_mount(NULL, USERPath, 1);
	
	/* 检测是否存在文件系统，如果没有则进行格式化 */
	} else if ( ret == FR_NO_FILESYSTEM ) {
		printf("NO FATFS, input f_mkfs to formatting...\r\n");
		
	} else {
		printf("f_mount success!\r\n");
		printf("size:xxx,name:xxx\r\n");
		u_str.sdInit = 1;
	}
}


void my_fatfs_test_fun( void ) {
	uint32_t counter = 0;
	UINT num;
	FRESULT ret;
	printf("mount\r\n");
	//挂载文件系统，挂载的时候会完成对应硬件设备（SD卡/SDnand）初始化
	ret = f_mount(&SDFatFS, USERPath, 1);
	printf("mount res:\r\n");
	if (ret != FR_OK) {
		printf("f_mount error!\r\n");
		goto mount_error;
		
	/* 检测是否存在文件系统，如果没有则进行格式化 */
	} else if ( ret == FR_NO_FILESYSTEM ) {
		printf("未检测到FATFS文件系统，执行格式化...\r\n");
		ret = f_mkfs(USERPath, 0, 0);
		if( ret == FR_OK ) {
			printf("格式化成功！\r\n");
			/* 先取消挂载，后重新挂载 */
			f_mount(NULL, USERPath, 1); 
			ret = f_mount(&SDFatFS, USERPath, 1);
		} else {
			printf("格式化失败！\r\n");
			goto mount_error;
		}
	} else {
		printf("f_mount success!\r\n");
	}
	
	/* 读写测试 */
	printf( "\r\n ========== write test ========== \r\n" );
	ret = f_open( &SDFile, "hh", FA_CREATE_ALWAYS | FA_WRITE );
	if( ret == FR_OK ) {
		__HAL_TIM_SET_AUTORELOAD( &htim4, 65534 );//设置定时器自动重装载的值, 到该值后重新计数
		__HAL_TIM_SET_COUNTER( &htim4, 0 );//设置定时器初始值
		HAL_TIM_Base_Start( &htim4 );//启动定时器
		//memset(temp_buf, 0, 1025);
		//memcpy(temp_buf, abc, 1024);
		//ret = f_write(&SDFile, abc, strlen((char*)abc), &num);
		counter = __HAL_TIM_GET_COUNTER(&htim4);//获取定时器当前计数值
		HAL_TIM_Base_Stop( &htim4 );//停止定时器
		printf("写入1024字节总用时: %d us", counter);
		f_close(&SDFile);\
	} 
	
	printf("\r\n ========== read test ==========\r\n");
	ret = f_open(&SDFile, "hh",FA_OPEN_EXISTING | FA_READ);
	if ( ret == FR_OK ) {
		printf("open file sucess!\r\n");
		ret = f_read(&SDFile, read_buf, 1024, &num);
		if ( ret == FR_OK ) {
			printf("read data:\"%s\"!\r\nread len：%d\r\n", read_buf, num);
		} else {
			printf("read error! ret:%d \r\n", ret);
			goto rw_error;
		}
	} else {
		printf("open file error!\r\n");
		goto rw_error;
	}

	rw_error:
	f_close(&SDFile);

	mount_error:
	f_mount(NULL, USERPath, 1);
}


void fatfs_test_fun ( void ) {
	//#define USERPath "0:/"
	//BYTE write_buf[] = "\r\n\r\nhello world!\r\n携手共进，推动人类科技的发展！！！\r\n";
	//BYTE read_buf[1024] = {0};
	UINT num;
	FRESULT ret;

	//挂载文件系统，挂载的时候会完成对应硬件设备（SD卡/SDnand）初始化
	ret = f_mount(&SDFatFS, USERPath, 1);
	if (ret != FR_OK) {
		printf("f_mount error!\r\n");
		goto mount_error;
		
	/* 检测是否存在文件系统，如果没有则进行格式化 */
	} else if ( ret == FR_NO_FILESYSTEM ) {
		printf("未检测到FATFS文件系统，执行格式化...\r\n");
		ret = f_mkfs(USERPath, 0, 0);
		if( ret == FR_OK ) {
			printf("格式化成功！\r\n");
			/* 先取消挂载，后重新挂载 */
			f_mount(NULL, USERPath, 1); 
			ret = f_mount(&SDFatFS, USERPath, 1);
		} else {
			printf("格式化失败！\r\n");
			goto mount_error;
		}
	} else {
		printf("f_mount success!\r\n");
	}
	
	/* 读写测试 */
	printf( "\r\n ========== write test ========== \r\n" );
	ret = f_open( &SDFile, "hello.txt", FA_CREATE_ALWAYS | FA_WRITE );
	if( ret == FR_OK ) {
		printf("open file sucess!\r\n");
		ret = f_write(&SDFile, write_buf, sizeof(write_buf)-1, &num);
		if( ret == FR_OK ) {
			printf( "write \r\n%s\r\n success!\r\nwrite len：%d\r\n", write_buf, num );
		} else {
			printf("write error! ret:%d \r\n", ret);
			goto rw_error;
		}
		f_close(&SDFile);
	} else {
		printf("open file error!\r\n");
		goto rw_error;
	}
	printf("\r\n ========== read test ==========\r\n");
	ret = f_open(&SDFile, "hello.txt",FA_OPEN_EXISTING | FA_READ);
	if ( ret == FR_OK ) {
		printf("open file sucess!\r\n");
		ret = f_read(&SDFile, read_buf, sizeof(read_buf), &num);
		if ( ret == FR_OK ) {
			printf("read data:\"%s\"!\r\nread len：%d\r\n", read_buf, num);
		} else {
			printf("read error! ret:%d \r\n", ret);
			goto rw_error;
		}
	} else {
		printf("open file error!\r\n");
		goto rw_error;
	}

	rw_error:
	f_close(&SDFile);

	mount_error:
	f_mount(NULL, USERPath, 1);

}



