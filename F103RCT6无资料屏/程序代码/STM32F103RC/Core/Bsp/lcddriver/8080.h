#ifndef __8080__H
#define __8080__H

#include "common.h"

#define LCD_CS_Low() HAL_GPIO_WritePin(CSN_GPIO_Port,CSN_Pin,GPIO_PIN_RESET)
#define LCD_CS_High() HAL_GPIO_WritePin(CSN_GPIO_Port,CSN_Pin,GPIO_PIN_SET)

#define LCD_RS_Low() HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_RESET)
#define LCD_RS_High() HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_SET) 

#define LCD_RD_Low() HAL_GPIO_WritePin(RD_GPIO_Port,RD_Pin,GPIO_PIN_RESET)
#define LCD_RD_High() HAL_GPIO_WritePin(RD_GPIO_Port,RD_Pin,GPIO_PIN_SET)

#define LCD_WR_Low() HAL_GPIO_WritePin(WR_GPIO_Port,WR_Pin,GPIO_PIN_RESET)
#define LCD_WR_High() HAL_GPIO_WritePin(WR_GPIO_Port,WR_Pin,GPIO_PIN_SET)

#define LCD_RST_Low() HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_RESET)
#define LCD_RST_High() HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_SET)


#define LCD_D0_H HAL_GPIO_WritePin(DB0_GPIO_Port,DB0_Pin,GPIO_PIN_SET)
#define LCD_D0_L HAL_GPIO_WritePin(DB0_GPIO_Port,DB0_Pin,GPIO_PIN_RESET)
#define LCD_D0() HAL_GPIO_ReadPin(DB0_GPIO_Port,DB0_Pin)

#define LCD_D1_H HAL_GPIO_WritePin(DB1_GPIO_Port,DB1_Pin,GPIO_PIN_SET)
#define LCD_D1_L HAL_GPIO_WritePin(DB1_GPIO_Port,DB1_Pin,GPIO_PIN_RESET)
#define LCD_D1() HAL_GPIO_ReadPin(DB1_GPIO_Port,DB1_Pin)

#define LCD_D2_H HAL_GPIO_WritePin(DB2_GPIO_Port,DB2_Pin,GPIO_PIN_SET)
#define LCD_D2_L HAL_GPIO_WritePin(DB2_GPIO_Port,DB2_Pin,GPIO_PIN_RESET)
#define LCD_D2() HAL_GPIO_ReadPin(DB2_GPIO_Port,DB2_Pin)

#define LCD_D3_H HAL_GPIO_WritePin(DB3_GPIO_Port,DB3_Pin,GPIO_PIN_SET)
#define LCD_D3_L HAL_GPIO_WritePin(DB3_GPIO_Port,DB3_Pin,GPIO_PIN_RESET)
#define LCD_D3() HAL_GPIO_ReadPin(DB3_GPIO_Port,DB3_Pin)

#define LCD_D4_H HAL_GPIO_WritePin(DB4_GPIO_Port,DB4_Pin,GPIO_PIN_SET)
#define LCD_D4_L HAL_GPIO_WritePin(DB4_GPIO_Port,DB4_Pin,GPIO_PIN_RESET)
#define LCD_D4() HAL_GPIO_ReadPin(DB4_GPIO_Port,DB4_Pin)

#define LCD_D5_H HAL_GPIO_WritePin(DB5_GPIO_Port,DB5_Pin,GPIO_PIN_SET)
#define LCD_D5_L HAL_GPIO_WritePin(DB5_GPIO_Port,DB5_Pin,GPIO_PIN_RESET)
#define LCD_D5() HAL_GPIO_ReadPin(DB5_GPIO_Port,DB5_Pin)

#define LCD_D6_H HAL_GPIO_WritePin(DB6_GPIO_Port,DB6_Pin,GPIO_PIN_SET)
#define LCD_D6_L HAL_GPIO_WritePin(DB6_GPIO_Port,DB6_Pin,GPIO_PIN_RESET)
#define LCD_D6() HAL_GPIO_ReadPin(DB6_GPIO_Port,DB6_Pin)

#define LCD_D7_H HAL_GPIO_WritePin(DB7_GPIO_Port,DB7_Pin,GPIO_PIN_SET)
#define LCD_D7_L HAL_GPIO_WritePin(DB7_GPIO_Port,DB7_Pin,GPIO_PIN_RESET)
#define LCD_D7() HAL_GPIO_ReadPin(DB7_GPIO_Port,DB7_Pin)



//uint16_t lcd_8080_read( uint8_t addr );
uint16_t ReadID(void);
void lcd_init( void );
uint16_t RGB_565(uint8_t red, uint8_t green, uint8_t blue);
void lcd_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
#endif /*__8080__H*/

