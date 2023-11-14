#include "../Bsp/bsp_fatfs.h"

void fatfs_test_fun ( void ) {
	#define USERPath "0:/"
	BYTE write_buf[] = "\r\n\r\nhello world!\r\n携手共进，推动人类科技的发展！！！\r\n";
	BYTE read_buf[1024] = {0};
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


