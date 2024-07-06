#include "8080.h"

static void data_out( uint8_t data );
static uint8_t data_in( void );
static void lcd_dgpio_write_mode( void );
static void lcd_dgpio_read_mode( void );
uint8_t Lcd_8080_ReadData( void );
void Lcd_8080_WriteCmd( uint16_t data );

//8080时序，读数据，实际分两步，第一步写入命令地址，第二步读数据
/**
 * 写cmd addr：
 * CS拉低，选中
 * DC/RS为低（读写命令）
 * RD拉高禁止读
 * WR拉低
 * 写入地址
 * WR拉高
 * 读：
 * //CS拉低，选中（已拉低）
 * DC/RS拉高（读写数据）
 * WR为高，禁止写
 * RD拉低
 * 读取数据
 * RD拉高
 * CS拉高，取消片选
*/
uint16_t lcd_8080_read( uint8_t addr ) {
    #define DELAY 0xff
    uint16_t data;
		
    LCD_CS_Low();

    //切换GPIO写模式
    lcd_dgpio_write_mode();
    //写addr
    LCD_RS_Low();//命令
    LCD_RD_High();//禁止读

    // data_out(0);//写数据
    // LCD_WR_Low();
    // 
    // LCD_WR_High();
    
    
    data_out(addr);//写数据
	LCD_WR_Low();
    
    LCD_WR_High();

    //切换GPIO读模式
    lcd_dgpio_read_mode();
    //读data
    LCD_RS_High();//数据
    LCD_WR_High();//禁止写
    LCD_RD_Low();
    
    data |= (data_in()<<8);//读数据
    LCD_RD_High();

		
    LCD_RD_Low();
    
    data |= (data_in());//读数据
    LCD_RD_High();

    LCD_CS_High();
		
	return data;
}



//查Driver code ID
uint16_t ReadID(void) {
    uint16_t id;
    Lcd_8080_WriteCmd(0x0000);
    Lcd_8080_ReadData();
    Lcd_8080_ReadData();
    id=Lcd_8080_ReadData();
    id=id<<8;
    id|=Lcd_8080_ReadData();
    return id;
}

void Lcd_8080_WriteCmd( uint16_t data ) {
    lcd_dgpio_write_mode();
    LCD_CS_Low();
    LCD_RS_Low();//命令
    LCD_RD_High();//禁止读
    
    data_out(data>>8);
    LCD_WR_Low();
    LCD_WR_High();

    data_out(data);
    LCD_WR_Low();
    LCD_WR_High();

    LCD_RS_High();
    LCD_CS_High();
}

void Lcd_8080_WriteData( uint8_t data ) {
    LCD_CS_Low();
    LCD_RS_High();//数据
    LCD_RD_High();//禁止读

    data_out(data);

    LCD_WR_Low();
    LCD_WR_High();
    LCD_CS_High();
}

uint8_t Lcd_8080_ReadData( void ) {
    uint8_t data;
    lcd_dgpio_read_mode();

    LCD_CS_Low();
    LCD_RS_High();//数据
    LCD_WR_High();//禁止写入

    LCD_RD_Low();
    Delay_Us(20);
    data = data_in();
    LCD_RD_High();

    LCD_CS_High();
	return data;
}

static void data_out( uint8_t data ) {
    if ( data & (1<<0) ) { 
        LCD_D0_H;
    } else {
        LCD_D0_L;
    }

    if ( data & (1<<1) ) { 
        LCD_D1_H;
    } else {
        LCD_D1_L;
    }

    if ( data & (1<<2) ) { 
        LCD_D2_H;
    } else {
        LCD_D2_L;
    }

    if ( data & (1<<3) ) { 
        LCD_D3_H;
    } else {
        LCD_D3_L;
    }

    if ( data & (1<<4) ) { 
        LCD_D4_H;
    } else {
        LCD_D4_L;
    }

    if ( data & (1<<5) ) { 
        LCD_D5_H;
    } else {
        LCD_D5_L;
    }

    if ( data & (1<<6) ) { 
        LCD_D6_H;
    } else {
        LCD_D6_L;
    }

    if ( data & (1<<7) ) { 
        LCD_D7_H;
    } else {
        LCD_D7_L;
    }
}

static uint8_t data_in( void ) {
    uint8_t data = 0;
    data = data | (LCD_D0()<<0);
    data = data | (LCD_D1()<<1);
    data = data | (LCD_D2()<<2);
    data = data | (LCD_D3()<<3);
    data = data | (LCD_D4()<<4);
    data = data | (LCD_D5()<<5);
    data = data | (LCD_D6()<<6);
    data = data | (LCD_D7()<<7);
    return data;
}

static void lcd_dgpio_write_mode( void ) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DB5_Pin|DB6_Pin|DB7_Pin|DB0_Pin|DB1_Pin|DB2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DB3_Pin|DB4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void lcd_dgpio_read_mode( void ) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = DB5_Pin|DB6_Pin|DB7_Pin|DB0_Pin|DB1_Pin|DB2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DB3_Pin|DB4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


void Lcd_8080_WriteReg( uint8_t cmd, uint16_t data ) {
    Lcd_8080_WriteCmd( cmd );
    Lcd_8080_WriteData( (uint8_t)(data >> 8) );
    Lcd_8080_WriteData( (uint8_t)data );
}

void lcd_setWindow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
		Lcd_8080_WriteReg(0x37, x1);
		Lcd_8080_WriteReg(0x36, x2);
		Lcd_8080_WriteReg(0x39, y1);
		Lcd_8080_WriteReg(0x38, y2);
		Lcd_8080_WriteReg(0x20, x1);
		Lcd_8080_WriteReg(0x21, y1);
}

uint16_t RGB_565(uint8_t red, uint8_t green, uint8_t blue) {
    return (red >> 3) << 11 | (green >> 2) << 5 | (blue >> 3);
}


void lcd_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    lcd_setWindow(x, y, x + w - 1, y + h - 1);
    Lcd_8080_WriteCmd(0x22);
    int i = w * h;
    while (i--) {
        Lcd_8080_WriteData( (uint8_t)(color >> 8) );
        Lcd_8080_WriteData( (uint8_t)color );
    }
}


void lcd_init( void ) {

    Lcd_8080_WriteReg( 0x01, 0x011C );
    Lcd_8080_WriteReg( 0x02, 0x0100 );
    Lcd_8080_WriteReg( 0x03, 0x1030 );
	Lcd_8080_WriteReg( 0x08, 0x0808 );
	Lcd_8080_WriteReg( 0x0C, 0x0000 );
	Lcd_8080_WriteReg( 0x0F, 0x0E01 );

    Lcd_8080_WriteReg( 0x10, 0x0A00 );
    Lcd_8080_WriteReg( 0x11, 0x1038 );
    Lcd_8080_WriteReg( 0xFF, 0x0003 );
    Lcd_8080_WriteReg( 0xB0, 0x1411 );
    Lcd_8080_WriteReg( 0xB1, 0x0202 );
    Lcd_8080_WriteReg( 0xB2, 0x0313 );
		
    Lcd_8080_WriteReg( 0x30, 0x0000 );
    Lcd_8080_WriteReg( 0x31, 0x00db );
    Lcd_8080_WriteReg( 0x32, 0x0000 );
    Lcd_8080_WriteReg( 0x33, 0x0000 );
    Lcd_8080_WriteReg( 0x34, 0x00db );
    Lcd_8080_WriteReg( 0x35, 0x0000 );
    Lcd_8080_WriteReg( 0x36, 0x00AF );
    Lcd_8080_WriteReg( 0x37, 0x0000 );
    Lcd_8080_WriteReg( 0x38, 0x00DB );
    Lcd_8080_WriteReg( 0x39, 0x0000 );
    
    Lcd_8080_WriteReg( 0xFF, 0x0003 );
    Lcd_8080_WriteReg( 0x50, 0x0000 );
    Lcd_8080_WriteReg( 0x51, 0x0300 );
    Lcd_8080_WriteReg( 0x52, 0x0103 );
    Lcd_8080_WriteReg( 0x53, 0x2011 );
    Lcd_8080_WriteReg( 0x54, 0x0703 );
    Lcd_8080_WriteReg( 0x55, 0x0000 );
    Lcd_8080_WriteReg( 0x56, 0x0400 );
    Lcd_8080_WriteReg( 0x57, 0x0107 );
    Lcd_8080_WriteReg( 0x58, 0x2011 );
    Lcd_8080_WriteReg( 0x59, 0x0703 );
    Lcd_8080_WriteReg( 0x20, 0x0000 );
    Lcd_8080_WriteReg( 0x21, 0x0000 );
    Lcd_8080_WriteReg( 0x07, 0x1017 );

}


