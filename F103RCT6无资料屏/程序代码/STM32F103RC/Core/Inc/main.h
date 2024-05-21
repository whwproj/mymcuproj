/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CSN_Pin GPIO_PIN_5
#define CSN_GPIO_Port GPIOA
#define WR_Pin GPIO_PIN_6
#define WR_GPIO_Port GPIOA
#define RD_Pin GPIO_PIN_7
#define RD_GPIO_Port GPIOA
#define DB3_Pin GPIO_PIN_4
#define DB3_GPIO_Port GPIOC
#define DB4_Pin GPIO_PIN_5
#define DB4_GPIO_Port GPIOC
#define DB5_Pin GPIO_PIN_0
#define DB5_GPIO_Port GPIOB
#define DB6_Pin GPIO_PIN_1
#define DB6_GPIO_Port GPIOB
#define DB7_Pin GPIO_PIN_2
#define DB7_GPIO_Port GPIOB
#define RESET_Pin GPIO_PIN_10
#define RESET_GPIO_Port GPIOB
#define BK_Pin GPIO_PIN_12
#define BK_GPIO_Port GPIOB
#define DB0_Pin GPIO_PIN_5
#define DB0_GPIO_Port GPIOB
#define DB1_Pin GPIO_PIN_6
#define DB1_GPIO_Port GPIOB
#define DB2_Pin GPIO_PIN_7
#define DB2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
