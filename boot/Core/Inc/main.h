/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
void entry_app(void);
void flicker_led(uint8_t group,uint8_t flicker,uint8_t sync,uint8_t keep,uint16_t delay);
void init_net_method(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY0_NETWORK_0_Pin GPIO_PIN_4
#define KEY0_NETWORK_0_GPIO_Port GPIOE
#define LED9_Pin GPIO_PIN_9
#define LED9_GPIO_Port GPIOF
#define LED10_Pin GPIO_PIN_10
#define LED10_GPIO_Port GPIOF
#define KEY1_NETWORK_1_Pin GPIO_PIN_0
#define KEY1_NETWORK_1_GPIO_Port GPIOA
#define WIFI2_RESET_Pin GPIO_PIN_10
#define WIFI2_RESET_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */
#define SmartConfig		1
extern uint8_t ENTRY_NETWORK_METHOD;// ƒ¨»œ≈‰Õ¯
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
