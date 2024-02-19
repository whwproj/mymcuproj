#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

extern	TIM_HandleTypeDef htim16;
extern	TIM_HandleTypeDef htim1;
extern	TIM_OC_InitTypeDef tim1SConfig;
	
void MX_TIM16_Init(void);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle);
	
void TIM1_PWM_Init(void);
	
void set_pwm_configchannel_1( uint8_t pulse );
void set_pwm_configchannel_2( uint8_t pulse );
void set_pwm_configchannel_3( uint8_t pulse );
void set_pwm_configchannel_4( uint8_t pulse );
	

#endif /*__TIM_H__*/

