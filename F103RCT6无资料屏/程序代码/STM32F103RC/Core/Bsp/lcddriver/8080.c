#include "8080.h"

//8080时序，读数据，实际分两步，第一步写入命令地址，第二步读数据
/**
 * 写cmd addr：
 * CS拉低，选中
 * DC/RS为低（写数据）
 * 等待WR为低，可以写入（写入数据）
 * WR拉高
 * 读：
 * //CS拉低，选中（已拉低）
 * DC/RS拉高（读数据）
 * WR为高，禁止写
 * 再RD的上升沿，读线上的数据 (D[0:7]) 8080并口
 * CS拉高，取消片选
*/
uint8_t lcd_8080_read( void ) {
    uint8_t data;
    //while(LCD_RD_High()){;;}
    LCD_CS_Low();
    LCD_RS_High();
    LCD_WR_High();
    while(LCD_RD_Low()){;;}
    data = lcd_8080_read_data();

}
static uint8_t lcd_8080_read_data( void ) {
    uint8_t data = 0;
    data = LCD_D0() << 
    if (  ) {

    }
}


