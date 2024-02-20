/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  }
#endif /* INCLUDE_xTaskGetSchedulerState */
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */
  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(NRF_IRQ_Pin);
  HAL_GPIO_EXTI_IRQHandler(STDBY_G_Pin);
  HAL_GPIO_EXTI_IRQHandler(CHRG_R_Pin);
	HAL_GPIO_EXTI_IRQHandler(IR_GPIO_Pin);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */
  /* USER CODE END EXTI4_15_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles ADC1 interrupt.
  */
void ADC1_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_IRQn 0 */

  /* USER CODE END ADC1_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_IRQn 1 */

  /* USER CODE END ADC1_IRQn 1 */
}

/**
  * @brief This function handles TIM16 global interrupt.
  */
void TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM16_IRQn 0 */

  /* USER CODE END TIM16_IRQn 0 */
  HAL_TIM_IRQHandler(&htim16);
  /* USER CODE BEGIN TIM16_IRQn 1 */

  /* USER CODE END TIM16_IRQn 1 */
}

/**
  * @brief This function handles TIM17 global interrupt.
  */
void TIM17_IRQHandler(void)
{
  /* USER CODE BEGIN TIM17_IRQn 0 */

  /* USER CODE END TIM17_IRQn 0 */
  HAL_TIM_IRQHandler(&htim17);
  /* USER CODE BEGIN TIM17_IRQn 1 */

  /* USER CODE END TIM17_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	BaseType_t phpt;
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
	if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) != RESET))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);  //清除空闲状态标志
		xTaskNotifyFromISR( debug_taskHandle, 1U<<PARSE_DATA, eSetBits,  &phpt );
		portYIELD_FROM_ISR( phpt );

	}
  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
	BaseType_t phpt;
	if ( GPIO_Pin == NRF_IRQ_Pin ) {
		HAL_NVIC_DisableIRQ(NRF_IRQ_EXTI_IRQn);
		nrf_str.heartTime = 0;
		xTaskNotifyFromISR( nrf_control_taskHandle, 1U<<NRF_PARSE_DATA, eSetBits, &phpt );
		portYIELD_FROM_ISR( phpt );
		
	} else if ( GPIO_Pin == STDBY_G_Pin ) {
		
	} else if ( GPIO_Pin == CHRG_R_Pin ) {
		
	} else if ( GPIO_Pin == IR_GPIO_Pin ) {
		static uint8_t idx;
		//采用280us中断一次 https://zhuanlan.zhihu.com/p/532388873  https://blog.csdn.net/STATEABC/article/details/131857097
		//NEC协议 引导码：低9ms(38Khz载波)~高4.5ms(空) -> 8000~10000us + 4000~5000us  最短12000us(42次中断) 最长15000us(54次中断)
		//逻辑“1”：低560us(38Khz载波) + 高1680us(空) = 2.24ms (8次中断)
		//逻辑“0”：低560us(38Khz载波) + 高560us(空) = 1.12ms (4次中断)
		//printf("..%d\r\n", ir_str.count);
		if ( ir_str.startFlag!=0 && (idx<32) && ir_str.count<42 ) {
			ir_str.irBitData[idx] = ir_str.count;
			__HAL_TIM_SET_COUNTER( &htim16, 0 );
			idx++;
			if ( idx >= 32 ) {
				idx = 0;
				ir_str.rxDr = 1;//一帧数据接收完毕
				ir_str.irRepet = 0;//非重复码
				ir_str.startFlag = 0;//解码结束
				xTaskNotifyFromISR( ir_taskHandle, 1U<<IR_DATA_PARSE, eSetBits, &phpt );
			}
		}
		if ( ir_str.count>=42 && ir_str.count<=54 && !ir_str.rxDr ) {//引导码
			//printf("解码开始\r\n");
			ir_str.startFlag = 1;//解码开始
			if ( ir_str.checkRepet>384 && ir_str.checkRepet<461 ) {//是重复码
				ir_str.rxDr = 1;//一帧数据接收完毕
				ir_str.irRepet = 1;//是重复码
				ir_str.startFlag = 0;//解码结束
				xTaskNotifyFromISR( ir_taskHandle, 1U<<IR_DATA_PARSE, eSetBits, &phpt );
			}
			ir_str.checkRepet = 1;//判断重复码开始
			idx = 0;
			__HAL_TIM_SET_COUNTER( &htim16, 0 );
		}
		ir_str.count = 0;
	}
}


/* USER CODE END 1 */
