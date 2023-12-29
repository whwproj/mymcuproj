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
#include "stm32g0xx_hal.h"

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
#define STDBY_G_Pin GPIO_PIN_14
#define STDBY_G_GPIO_Port GPIOC
#define STDBY_G_EXTI_IRQn EXTI4_15_IRQn
#define CHRG_R_Pin GPIO_PIN_15
#define CHRG_R_GPIO_Port GPIOC
#define CHRG_R_EXTI_IRQn EXTI4_15_IRQn
#define BAT_Pin GPIO_PIN_0
#define BAT_GPIO_Port GPIOA
#define NRF_CE_Pin GPIO_PIN_3
#define NRF_CE_GPIO_Port GPIOA
#define NRF_IRQ_Pin GPIO_PIN_4
#define NRF_IRQ_GPIO_Port GPIOA
#define NRF_IRQ_EXTI_IRQn EXTI4_15_IRQn
#define NRF_CSN_Pin GPIO_PIN_7
#define NRF_CSN_GPIO_Port GPIOA
#define PWM_LED_Pin GPIO_PIN_0
#define PWM_LED_GPIO_Port GPIOB
#define HXCTL_Pin GPIO_PIN_11
#define HXCTL_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
