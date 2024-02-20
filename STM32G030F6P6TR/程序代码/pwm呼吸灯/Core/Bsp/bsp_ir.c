#include "../Bsp/bsp_ir.h"

IR_STR ir_str = {
	 .rxDr = 0U,
	 .startFlag = 0U,
	 .count = 0U
};


//采用280us中断一次 https://zhuanlan.zhihu.com/p/532388873
//NEC协议 引导码：低9ms(38Khz载波)~高4.5ms(空) -> 8000~10000us + 4000~5000us  最短12000us(42次中断) 最长15000us(54次中断)
//逻辑“1”：低560us(38Khz载波) + 高1680us(空) = 2.24ms (8次中断)
//逻辑“0”：低560us(38Khz载波) + 高560us(空) = 1.12ms (4次中断)
//LSB优先（最先接收到最低位）一般存储采用小端模式
int8_t ir_data_parse( void ) {
	uint8_t i, j, k, dat;
	for ( i=0,k=0; i<4; i++ ) {
		dat = 0;
		for ( j=0; j<8; j++ ) {
			if ( ir_str.irBitData[k] == 4 ) {//逻辑0
				//dat |= 0<<j;
			} else if ( ir_str.irBitData[k] == 8 ) {//逻辑1
				dat |= (1<<j);
			} else {//错误码
				return -1;
			}
			k++;
		}
		ir_str.irData[i] = dat;
	}
	return 0;
}

void execute_led( void ) {
	uint16_t temp;
	temp = ir_str.irData[2]<<8 | ir_str.irData[3];
	if ( temp==LIGHT_ENHANCE_1 || temp==LIGHT_ENHANCE_2 ) {//调亮
		xTaskNotify( pwm_taskHandle, 1U<<LIGHT_ENHANCE, eSetBits );
		
	} else if ( temp==LIGHT_WEAKEN_1 || temp==LIGHT_WEAKEN_2 ) {//调暗
		xTaskNotify( pwm_taskHandle, 1U<<LIGHT_WEAKEN, eSetBits );
		
	} else if ( temp==LIGHT_TOOGLE_1 || temp==LIGHT_TOOGLE_2 ) {//开关切换
		if ( pwm_str.lightSta == 1 ) {//当前开启，关闭
			xTaskNotify( pwm_taskHandle, 1U<<LED_OFF, eSetBits );
			if ( temp == LIGHT_TOOGLE_2 ) {
				//vTaskDelay(100);
				xTaskNotify( pwm_taskHandle, 1U<<POWER_OFF, eSetBits );
			}
		} else {//当前关闭，开启
			if ( HAL_GPIO_ReadPin( HXCTL_GPIO_Port, HXCTL_Pin ) == GPIO_PIN_RESET ) {//先打开电源
				xTaskNotify( pwm_taskHandle, 1U<<POWER_ON, eSetBits );
				//vTaskDelay(100);
			}
			xTaskNotify( pwm_taskHandle, 1U<<LED_ON, eSetBits );
		}
		
	} else if ( temp == LED_MODE ) {//模式切换
		pwm_str.mode = pwm_str.mode>=1 ? 0 : (pwm_str.mode+1);
		if ( pwm_str.mode == 0 ) {//单亮模式
			xTaskNotify( pwm_taskHandle, 1U<<LIGHT_ENHANCE, eSetBits );
			xTaskNotify( pwm_taskHandle, 1U<<LIGHT_WEAKEN, eSetBits );
			
		} else if ( pwm_str.mode == 1 ) {//呼吸灯模式
			xTaskNotify( pwm_taskHandle, 1U<<PWM_FLASHING, eSetBits );
			
		} else if ( pwm_str.mode == 2 ) {//闪烁模式
		
		}
		pwm_str.lightSta = 1;
	}
}   	
    			
 		
 	
 	










