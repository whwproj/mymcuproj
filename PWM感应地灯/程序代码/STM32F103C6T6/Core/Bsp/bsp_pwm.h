#ifndef __BSP_PWM__H
#define __BSP_PWM__H

#include "../Bsp/common.h"

typedef struct _PWM_STR {
	
	uint8_t lightSta;//0:关 1:开 2:闪烁
	uint8_t maxBtnPer;//最高亮度百分比 50~100

} PWM_STR;

/*---- pwm task bits start ----------------*/
#define TURN_ON			20
#define TURN_OFF		21
#define FLASHING		22
/*---- pwm task bits start ----------------*/
	
			
extern PWM_STR pwm_str;


void pwm_flashing ( void );
void pwm_turn_off ( void );
void pwm_turn_on ( void );
				

#endif /*__BSP_PWM__H*/




