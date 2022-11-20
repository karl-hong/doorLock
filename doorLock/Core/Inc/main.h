/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

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
#define gLed_Pin GPIO_PIN_0
#define gLed_GPIO_Port GPIOB
#define rLed_Pin GPIO_PIN_1
#define rLed_GPIO_Port GPIOB
#define motorD_Pin GPIO_PIN_2
#define motorD_GPIO_Port GPIOB
#define motorC_Pin GPIO_PIN_10
#define motorC_GPIO_Port GPIOB
#define motorB_Pin GPIO_PIN_11
#define motorB_GPIO_Port GPIOB
#define motorA_Pin GPIO_PIN_12
#define motorA_GPIO_Port GPIOB
#define doorDetect2_Pin GPIO_PIN_14
#define doorDetect2_GPIO_Port GPIOB
#define doorDetect2_EXTI_IRQn EXTI4_15_IRQn
#define doorDetect1_Pin GPIO_PIN_15
#define doorDetect1_GPIO_Port GPIOB
#define doorDetect1_EXTI_IRQn EXTI4_15_IRQn
#define gsensor_SCL_Pin GPIO_PIN_6
#define gsensor_SCL_GPIO_Port GPIOF
#define gsensor_SDA_Pin GPIO_PIN_7
#define gsensor_SDA_GPIO_Port GPIOF
#define light2_Pin GPIO_PIN_3
#define light2_GPIO_Port GPIOB
#define light1_Pin GPIO_PIN_4
#define light1_GPIO_Port GPIOB
#define lock_Detect1_Pin GPIO_PIN_6
#define lock_Detect1_GPIO_Port GPIOB
#define lock_Detect1_EXTI_IRQn EXTI4_15_IRQn
#define lock_Detect2_Pin GPIO_PIN_7
#define lock_Detect2_GPIO_Port GPIOB
#define lock_Detect2_EXTI_IRQn EXTI4_15_IRQn
#define KEY_Detect_Pin GPIO_PIN_8
#define KEY_Detect_GPIO_Port GPIOB
#define KEY_Detect_EXTI_IRQn EXTI4_15_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
