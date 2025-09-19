/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include <stdbool.h>
#include "tmcm1636.h"
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
void dynoUncalibratedFall(void);
void dynoCalibratedFall(void);
void dynoIsotonicTest(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define NOT_TRIP_Pin GPIO_PIN_0
#define NOT_TRIP_GPIO_Port GPIOB
#define RELAY_OBSERVE_Pin GPIO_PIN_1
#define RELAY_OBSERVE_GPIO_Port GPIOB
#define RELAY_OBSERVE_EXTI_IRQn EXTI1_IRQn
#define ENERGIZE_Pin GPIO_PIN_15
#define ENERGIZE_GPIO_Port GPIOB
#define RED_LED_Pin GPIO_PIN_14
#define RED_LED_GPIO_Port GPIOD
#define SIGNAL_OUT_1_Pin GPIO_PIN_5
#define SIGNAL_OUT_1_GPIO_Port GPIOD
#define SIGNAL_OUT_2_Pin GPIO_PIN_7
#define SIGNAL_OUT_2_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define TMCM1636_CAN_ID 0x04 // Worth double checking
#define ARDUINO_CAN_ID 0x50
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
