#ifndef __BSP_LED__H
#define __BSP_LED__H

#include "../common.h"

void pwm_on( void );
void pwm_off( void );
void pwm_flicker( void );
void pwm_set_speed( uint16_t n );


#endif /*__BSP_LED__H*/

