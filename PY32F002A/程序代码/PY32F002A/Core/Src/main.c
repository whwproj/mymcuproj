/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/
#include "common.h"
/* Private variables ---------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);

uint16_t temp_t;

/**
  * @brief  The application entry point
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick */
  HAL_Init();
  
  /* Configure the system clock */
	APP_SystemClockConfig(); 
	
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	
	MX_TIM16_Init();
	HAL_TIM_Base_Start_IT( &htim16 );
	HAL_TIM_Base_Start( &htim16 );
	__HAL_TIM_SET_COUNTER( &htim16, 0 );
	
	TIM1_PWM_Init();
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);//通道4开始输出PWM
	
  /* Infinite loop */
  printf("init ok\r\n");
  while (1)
  {

//	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET );
//	  HAL_Delay(1000);
//	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET );
//	  HAL_Delay(1000);
//	  printf("uart send\r\n");
	  if ( ir_str.rxDr != 0 ) {
		ir_str.rxDr = 0;
		if ( ri_data_parse() == 0 ) {
			//printf("接收到用户码: 0x%.2X 0x%.2X  ", ir_str.irData[0], ir_str.irData[1]);
			//printf("接收到键值码: 0x%.2X 0x%.2X\r\n", ir_str.irData[2], ir_str.irData[3]);
			execute_led();
		} else {
			//printf("ErrCode\r\n");
		}
	}
  }
}


void HAL_GPIO_EXTI_Callback ( uint16_t GPIO_Pin ) {
	static uint8_t idx;
	//采用280us中断一次 https://zhuanlan.zhihu.com/p/532388873
	//NEC协议 引导码：低9ms(38Khz载波)~高4.5ms(空) -> 8000~10000us + 4000~5000us  最短12000us(42次中断) 最长15000us(54次中断)
	//逻辑“1”：低560us(38Khz载波) + 高1680us(空) = 2.24ms (8次中断)
	//逻辑“0”：低560us(38Khz载波) + 高560us(空) = 1.12ms (4次中断)
	//printf("..%d\r\n", ir_str.count);
	if ( ir_str.startFlag != 0 && (idx<32)) {
		ir_str.irBitData[idx] = ir_str.count;
		__HAL_TIM_SET_COUNTER( &htim16, 0 );
		idx++;
		if ( idx >= 32 ) {
			ir_str.startFlag = 0;//解码结束
			idx = 0;
			ir_str.rxDr = 1;//一帧数据接收完毕
		}
	}
	if ( ir_str.count>=42 && ir_str.count<=54 && !ir_str.rxDr ) {//引导码
		//printf("解码开始\r\n");
		ir_str.startFlag = 1;//解码开始
		idx = 0;
		__HAL_TIM_SET_COUNTER( &htim16, 0 );
	}
	ir_str.count = 0;
}

void HAL_TIM_PeriodElapsedCallback ( TIM_HandleTypeDef *htim ) {
	if ( htim == &htim16 ) {
		ir_str.count++;
	}
}

/**
  * @brief  System Clock Configuration
  * @param  None
  * @retval None
  */
static void APP_SystemClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                          /* Enable HSI */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                          /* HSI clock 1 division */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_8MHz;  /* Config HSI 8MHz Calibration */
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                         /* Disable HSE */
  /*RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;*/
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;                         /* Disable LSI */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  /* Configure Clock */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; /* System clock selection HSI */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;     /* AHB clock 1 division */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;      /* APB clock 1 division */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void APP_ErrorHandler(void)
{
  /* Infinite loop */
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     for example: printf("Wrong parameters value: file %s on line %d\r\n", file, line)  */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
