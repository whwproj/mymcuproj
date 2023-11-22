#include "../Bsp/bsp_oled.h"
#include "../Bsp/OLED_Font.h"

void olde_Write_cmd( uint8_t cmd ) {
	HAL_I2C_Mem_Write( &hi2c1, oledAddr, cmdMode, 1, &cmd, 1, portMAX_DELAY);
//	uint8_t data[2];
//	data[0] = 0;
//	data[1] = cmd;
//	HAL_I2C_Master_Transmit( &hi2c1, 0x78, data, 2, 0x100 );
	
}

void olde_Write_data( uint8_t dat ) {
	HAL_I2C_Mem_Write( &hi2c1, oledAddr, datMode, 1, &dat, 1, portMAX_DELAY);
//	uint8_t data[2];
//	data[0] = 0x40;
//	data[1] = dat;
//	HAL_I2C_Master_Transmit( &hi2c1, 0x78, data, 2, 0x100 );
}

void oled_clear( void ) {
	for( int i=0; i<8; i++ ){
		olde_Write_cmd(0xB0 + i);//page0--page7
		//每个page从0列
		olde_Write_cmd(0x00);
		olde_Write_cmd(0x10);
		//0到127列，依次写入0，每写入数据，列地址自动偏移
		for( int j=0; j<128; j++ ){
			olde_Write_data( 0 );
		}
	}
}

void oled_init( void ) {
//	olde_Write_cmd(0xAE);//--display off
//	olde_Write_cmd(0x00);//---set low column address
//	olde_Write_cmd(0x10);//---set high column address
//	olde_Write_cmd(0x40);//--set start line address
//	olde_Write_cmd(0xB0);//--set page address
//	olde_Write_cmd(0x81);// contract control
//	olde_Write_cmd(0xFF);//--128
//	olde_Write_cmd(0xA1);//set segment remap
//	olde_Write_cmd(0xA6);//--normal / reverse
//	olde_Write_cmd(0xA8);//--set multiplex ratio(1 to 64)
//	olde_Write_cmd(0x3F);//--1/32 duty
//	olde_Write_cmd(0xC8);//Com scan direction
//	olde_Write_cmd(0xD3);//-set display offset
//	olde_Write_cmd(0x00);//
//	olde_Write_cmd(0xD5);//set osc division
//	olde_Write_cmd(0x80);//
//	olde_Write_cmd(0xD8);//set area color mode off
//	olde_Write_cmd(0x05);//
//	olde_Write_cmd(0xD9);//Set Pre-Charge Period
//	olde_Write_cmd(0xF1);//
//	olde_Write_cmd(0xDA);//set com pin configuartion
//	olde_Write_cmd(0x12);//
//	olde_Write_cmd(0xDB);//set Vcomh
//	olde_Write_cmd(0x30);//
//	olde_Write_cmd(0x8D);//set charge pump enable
//	olde_Write_cmd(0x14);//
	uint32_t i, j;
	uint8_t pData = 0xFF;

	for (i = 0; i < 1000; i++) {//上电延时
		for (j = 0; j < 1000; j++)
			;
	}

	//OLED_I2C_Init(); //端口初始化
	printf( "strt:pData: %d", pData );
	//HAL_I2C_Mem_Read(&hi2c1, 0x78, 0, 1, &pData, 1, 0x100);
	HAL_I2C_Master_Receive( &hi2c1, 0x78, &pData, 1, 0x100 );
	printf( "  end:pData: %d\r\n", pData );
	//HAL_I2C_Master_Receive()

	olde_Write_cmd(0xAE); //关闭显示
	olde_Write_cmd(0xD5); //设置显示时钟分频比/振荡器频率
	olde_Write_cmd(0x80);
	
	olde_Write_cmd(0xA8); //设置多路复用率
	olde_Write_cmd(0x3F);

	olde_Write_cmd(0xD3); //设置显示偏移
	olde_Write_cmd(0x34);
	
	olde_Write_cmd(0x40); //设置显示开始行

	olde_Write_cmd(0xA1); //设置左右方向，0xA1正常 0xA0左右反置

	olde_Write_cmd(0xC8); //设置上下方向，0xC8正常 0xC0上下反置

	olde_Write_cmd(0xDA); //设置COM引脚硬件配置
	olde_Write_cmd(0x12);

	olde_Write_cmd(0x81); //设置对比度控制
	olde_Write_cmd(0xCF);

	olde_Write_cmd(0xD9); //设置预充电周期
	olde_Write_cmd(0xF1);

	olde_Write_cmd(0xDB); //设置VCOMH取消选择级别
	olde_Write_cmd(0x30);

	olde_Write_cmd(0xA4); //设置整个显示打开/关闭

	olde_Write_cmd(0xA6); //设置正常/倒转显示

	olde_Write_cmd(0x8D); //设置充电泵
	olde_Write_cmd(0x14);

	olde_Write_cmd(0xAF); //开启显示

	oled_clear(); // OLED清屏
}



/**
 * @brief  OLED设置光标位置
 * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
 * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
 * @retval 无
 */
void OLED_SetCursor(uint8_t Y, uint8_t X) {
	olde_Write_cmd(0xB0 | Y);				 //设置Y位置
	olde_Write_cmd(0x10 | ((X & 0xF0) >> 4));//设置X位置低4位
	olde_Write_cmd(0x00 | (X & 0x0F));		 //设置X位置高4位
}

/**
 * @brief  OLED显示一个字符
 * @param  Line 行位置，范围：1~4
 * @param  Column 列位置，范围：1~16
 * @param  Char 要显示的一个字符，范围：ASCII可见字符
 * @retval 无
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); //设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		olde_Write_data(OLED_F8x16[Char - ' '][i]); //显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); //设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		olde_Write_data(OLED_F8x16[Char - ' '][i + 8]); //显示下半部分内容
	}
}

/**
 * @brief  OLED显示一个汉字
 * @param  chineseIdex 要显示的一个汉字库中的索引
 * @retval 无
 */
void OLED_ShowWord_3( uint8_t idx, uint8_t Line, uint8_t chineseIdex) {
	uint8_t i;
	OLED_SetCursor( Line*2, 40+idx*16 ); //设置光标位置在上半部分
	for ( i=0; i<16; i++ ) {
		olde_Write_data(OLED_F16x16[chineseIdex][i]); //显示上半部分内容
	}
	OLED_SetCursor( Line*2+1, 40+idx*16 ); //设置光标位置在上半部分
	for ( i=0; i<16; i++ ) {
		olde_Write_data(OLED_F16x16[chineseIdex][i+16]); //显示上半部分内容
	}
}

void oled_show_yinxiang( void ) {
	for ( int j=0; j<6; j++ ) {
		OLED_SetCursor( j, 27 );
		for ( int i=0; i<74; i++ ) {
			olde_Write_data(oled_yinxiang[j*74+i]);
		}
	}
}

void oled_show_pinzideng( void ) {
	for ( int j=0; j<5; j++ ) {
		OLED_SetCursor( j, 7 );
		for ( int i=0; i<113; i++ ) {
			olde_Write_data(oled_pinzideng[j*113+i]);
		}
	}
}

void oled_test( void ) {
	oled_init(); //初始化Oled
//	for ( int j=0; j<8; j++ ) {
//		for ( int i=0; i<60; i++ ) {
//			OLED_SetCursor(j, 30+i );
//			olde_Write_data(0xFF);
//		}
//	}
	
//	8  0 0 0 0 0 0 0 0 
//	16*3 = 48
//	128 - 48 = 80
//	80 / 2 = 40
//	OLED_ShowWord_3(0, 0, 5);
//	OLED_ShowWord_3(1, 0, 6);
//	OLED_ShowWord_3(2, 0, 7);
	while (1) {
		oled_show_pinzideng();
		vTaskDelay(2000);
		oled_clear(); // OLED清屏
		
		oled_show_yinxiang();
		vTaskDelay(2000);
		oled_clear(); // OLED清屏
	}
}





















