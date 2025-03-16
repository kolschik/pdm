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
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_iwdg.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_gpio.h"

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
#define LED_Pin LL_GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define OUT1_CS_Pin LL_GPIO_PIN_0
#define OUT1_CS_GPIO_Port GPIOA
#define OUT1_SEN_Pin LL_GPIO_PIN_1
#define OUT1_SEN_GPIO_Port GPIOA
#define OUT1_H_Pin LL_GPIO_PIN_2
#define OUT1_H_GPIO_Port GPIOA
#define OUT2_H_Pin LL_GPIO_PIN_3
#define OUT2_H_GPIO_Port GPIOA
#define OUT2_SEN_Pin LL_GPIO_PIN_4
#define OUT2_SEN_GPIO_Port GPIOA
#define OUT2_CS_Pin LL_GPIO_PIN_5
#define OUT2_CS_GPIO_Port GPIOA
#define OUT34_CS_Pin LL_GPIO_PIN_6
#define OUT34_CS_GPIO_Port GPIOA
#define OUT56_CS_Pin LL_GPIO_PIN_7
#define OUT56_CS_GPIO_Port GPIOA
#define BAT_METER_Pin LL_GPIO_PIN_0
#define BAT_METER_GPIO_Port GPIOB
#define WATER_DET_Pin LL_GPIO_PIN_12
#define WATER_DET_GPIO_Port GPIOB
#define OUT6_H_Pin LL_GPIO_PIN_14
#define OUT6_H_GPIO_Port GPIOB
#define OUT56_SEL_Pin LL_GPIO_PIN_15
#define OUT56_SEL_GPIO_Port GPIOB
#define OUT56_SEN_Pin LL_GPIO_PIN_8
#define OUT56_SEN_GPIO_Port GPIOA
#define OUT5_EN_Pin LL_GPIO_PIN_9
#define OUT5_EN_GPIO_Port GPIOA
#define OUT4_SEN_Pin LL_GPIO_PIN_10
#define OUT4_SEN_GPIO_Port GPIOA
#define OUT4_H_Pin LL_GPIO_PIN_3
#define OUT4_H_GPIO_Port GPIOB
#define OUT3_SEN_Pin LL_GPIO_PIN_4
#define OUT3_SEN_GPIO_Port GPIOB
#define OUT3_H_Pin LL_GPIO_PIN_5
#define OUT3_H_GPIO_Port GPIOB
#define OUT3_L_Pin LL_GPIO_PIN_6
#define OUT3_L_GPIO_Port GPIOB
#define OUT4_L_Pin LL_GPIO_PIN_7
#define OUT4_L_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
