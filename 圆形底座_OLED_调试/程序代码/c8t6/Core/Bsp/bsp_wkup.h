#ifndef __BSP_WKUP__H
#define __BSP_WKUP__H

#include "../common.h"


uint8_t Wkup0_Check_Up_OS( void );
uint8_t Wkup0_Check_Up_HAL( void );
void enter_standbyMode( void );
void judge_if_Rest(void);
#endif /*__BSP_WKUP__H*/



