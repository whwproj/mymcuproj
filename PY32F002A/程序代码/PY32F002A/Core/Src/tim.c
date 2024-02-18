#include "tim.h"

TIM_HandleTypeDef htim16;

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

