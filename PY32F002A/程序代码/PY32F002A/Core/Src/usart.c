#include "usart.h"

UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void) {
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK) {
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle) 
	{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==USART1) {
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      //Error_Handler();
    }

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    //PF0     ------> USART1_RX
    //PF1     ------> USART1_TX
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART1;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle) {
  if(uartHandle->Instance==USART1) {
    __HAL_RCC_USART1_CLK_DISABLE();
    //PF0     ------> USART1_RX
    //PF1     ------> USART1_TX
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0|GPIO_PIN_1);
  }
}


int fputc(int ch, FILE *f) {	
  HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, 0xffff);
	return 0;
}


