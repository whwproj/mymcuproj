#include "tim.h"

TIM_HandleTypeDef htim16;

TIM_HandleTypeDef htim1;
TIM_OC_InitTypeDef tim1SConfig;

#define  PERIOD_VALUE       (uint32_t)(100 - 1)
#define  PULSE1_VALUE       20
#define  PULSE2_VALUE       40
#define  PULSE3_VALUE       60
#define  PULSE4_VALUE       80

static void APP_TIM1_INIT( void );
static void APP_TIM1_PWM( void );

/**
  * @brief  错误执行函数
  * @param  无
  * @retval 无
  */
void Error_Handler(void) {
  /* 无限循环 */
  while (1) { }
}

void MX_TIM16_Init(void) {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 8-1;//8M / 8 = 1us计数一次
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 280-1-20;//多20us容差
  //采用280us中断一次 https://zhuanlan.zhihu.com/p/532388873
  //NEC协议 引导码：低9ms(38Khz载波)~高4.5ms(空) -> 8000~10000us + 4000~5000us  最短12000us(42次中断) 最长15000us(54次中断)
  //逻辑“1”：低560us(38Khz载波) + 高1680us(空) = 2.24ms (8次中断)
  //逻辑“0”：低560us(38Khz载波) + 高560us(空) = 1.12ms (4次中断)
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK) {
    //Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim16, &sClockSourceConfig) != HAL_OK) {
    //Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim16, &sMasterConfig) != HAL_OK) {
    //Error_Handler();
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle) {
  if(tim_baseHandle->Instance==TIM16) {
    __HAL_RCC_TIM16_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM16_IRQn);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle) {
  if(tim_baseHandle->Instance==TIM16) {
    __HAL_RCC_TIM16_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TIM16_IRQn);
  }
}


void TIM1_PWM_Init(void) {
	__HAL_RCC_TIM1_CLK_ENABLE(); 
	APP_TIM1_INIT();
	APP_TIM1_PWM();
}

/**
  * @brief  TIM1初始化函数
  * @param  无
  * @retval 无
  */
static void APP_TIM1_INIT( void ) {
  /* 选择TIM1 */
  htim1.Instance = TIM1;                                                  
  /* 自动重装载值 */
  htim1.Init.Period            = 100;                                     
  /* 预分频为800-1 */
  htim1.Init.Prescaler         = 400 - 1;//8Mhz  8000000/(400*100) = 200Hz
  /* 时钟不分频 */
  htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;                  
  /* 向上计数*/
  htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;                      
  /* 不重复计数 */
  htim1.Init.RepetitionCounter = 1 - 1;                                   
  /* 自动重装载寄存器没有缓冲 */
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;          
  /* 基础时钟初始化 */
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
    Error_Handler();
  }

}
/**
  * @brief  TIM1 PWM配置
  * @param  无
  * @retval 无
  */
static void APP_TIM1_PWM( void ) {
  /*输出配置为翻转模式 */
  tim1SConfig.OCMode       = TIM_OCMODE_PWM1;                                     
  /*OC通道输出高电平有效 */
  tim1SConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;                                 
  /*输出快速使能关闭 */
  tim1SConfig.OCFastMode   = TIM_OCFAST_DISABLE;                                  
  /*OCN通道输出高电平有效 */
  tim1SConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;                                
  /*空闲状态OC1N输出低电平 */
  tim1SConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;                              
  /*空闲状态OC1输出低电平*/
  tim1SConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;                               

//  /*CC1值为10，占空比=10/50=20%*/
//  sConfig.Pulse = PULSE1_VALUE;                                              
//  /* 通道1配置 */
//  if (HAL_TIM_PWM_ConfigChannel(&htimx, &sConfig, TIM_CHANNEL_1) != HAL_OK)
//  {
//    Error_Handler();
//  }

//  /* CC2值为20,占空比=20/50=40% */
//  sConfig.Pulse = PULSE2_VALUE;                                               
//  /* 通道2配置 */
//  if (HAL_TIM_PWM_ConfigChannel(&htimx, &sConfig, TIM_CHANNEL_2) != HAL_OK)
//  {
//    Error_Handler();
//  }

//  /*CC3值为30,占空比=30/50=60%*/
//  sConfig.Pulse = PULSE3_VALUE;                                              
//  /* 通道3配置 */
//  if (HAL_TIM_PWM_ConfigChannel(&htimx, &sConfig, TIM_CHANNEL_3) != HAL_OK)
//  {
//    /* Configuration Error */
//    Error_Handler();
//  }

  /* CC4值为40,占空比=40/50=80% */
  tim1SConfig.Pulse = PERIOD_VALUE;
  /*通道4配置 */
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &tim1SConfig, TIM_CHANNEL_4) != HAL_OK) {
    Error_Handler();
  }
}

void set_pwm_configchannel_1( uint8_t pulse ) {
	tim1SConfig.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(&htim1, &tim1SConfig, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}
void set_pwm_configchannel_2( uint8_t pulse ) {
	tim1SConfig.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(&htim1, &tim1SConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}
void set_pwm_configchannel_3( uint8_t pulse ) {
	tim1SConfig.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(&htim1, &tim1SConfig, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}
void set_pwm_configchannel_4( uint8_t pulse ) {
	tim1SConfig.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(&htim1, &tim1SConfig, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
}

