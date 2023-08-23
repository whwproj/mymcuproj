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
#include "stm32f1xx_hal.h"

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
//PB15 MOSI		PA7
//PB14 MISO		PA6
//PB13 SCK		PA5
//PB8	 CE			PC5
//PB9	 IRQ		PC4
//PB12 CSN		PC6
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI1_IRQ_Pin GPIO_PIN_4
#define SPI1_IRQ_GPIO_Port GPIOC
#define SPI1_IRQ_EXTI_IRQn EXTI4_IRQn
#define NRF_CE_Pin GPIO_PIN_5
#define NRF_CE_GPIO_Port GPIOC
#define SPI2_CSN_Pin GPIO_PIN_12
#define SPI2_CSN_GPIO_Port GPIOB
#define NRF_CSN_Pin GPIO_PIN_6
#define NRF_CSN_GPIO_Port GPIOC
#define SPI2_CE_Pin GPIO_PIN_8
#define SPI2_CE_GPIO_Port GPIOB
#define SPI2_IRQ_Pin GPIO_PIN_9
#define SPI2_IRQ_GPIO_Port GPIOB
#define SPI2_IRQ_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
