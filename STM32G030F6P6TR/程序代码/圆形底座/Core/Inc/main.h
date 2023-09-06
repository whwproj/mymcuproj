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
#define BLT_STA_Pin GPIO_PIN_9
#define BLT_STA_GPIO_Port GPIOB
#define BLT_AT_EN_Pin GPIO_PIN_15
#define BLT_AT_EN_GPIO_Port GPIOC
#define IPS_CS_Pin GPIO_PIN_3
#define IPS_CS_GPIO_Port GPIOA
#define IPS_DC_Pin GPIO_PIN_5
#define IPS_DC_GPIO_Port GPIOA
#define IPS_BLK_Pin GPIO_PIN_7
#define IPS_BLK_GPIO_Port GPIOA
#define IPS_CSA8_Pin GPIO_PIN_8
#define IPS_CSA8_GPIO_Port GPIOA
#define W25Q_CS_Pin GPIO_PIN_12
#define W25Q_CS_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
