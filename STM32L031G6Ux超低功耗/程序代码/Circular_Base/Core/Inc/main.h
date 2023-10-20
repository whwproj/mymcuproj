/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NRF_CS_Pin GPIO_PIN_14
#define NRF_CS_GPIO_Port GPIOC
#define NRF_CE_Pin GPIO_PIN_15
#define NRF_CE_GPIO_Port GPIOC
#define NRF_IRQ_Pin GPIO_PIN_0
#define NRF_IRQ_GPIO_Port GPIOA
#define ADC_Pin GPIO_PIN_1
#define ADC_GPIO_Port GPIOA
#define APDS9930_INT_Pin GPIO_PIN_2
#define APDS9930_INT_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOA
#define PWM_LED_Pin GPIO_PIN_6
#define PWM_LED_GPIO_Port GPIOA
#define W25Q_CS_Pin GPIO_PIN_1
#define W25Q_CS_GPIO_Port GPIOB
#define PWM_IR_Pin GPIO_PIN_8
#define PWM_IR_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOB
#define DHT22_Pin GPIO_PIN_7
#define DHT22_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
