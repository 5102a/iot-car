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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
// 设备状态结构体
typedef struct
{
	/*只读*/
	/*电机*/
	// 速度
	uint16_t cur_speed_whole;// 当前整体车速，cm/s
	uint16_t cur_speed_angle;// 移动角度
	int16_t cur_speed_lq;// 左前轮速度
	int16_t cur_speed_rq;// 右前轮速度
	int16_t cur_speed_lh;// 左后轮速度
	int16_t cur_speed_rh;// 右后轮速度
	
	/*读写*/
	// 设置速度
	uint16_t set_speed_whole;// 当前整体车速，cm/s
	uint16_t set_speed_angle;// 移动角度	
	int16_t set_speed_dir;// 旋转方向
	uint16_t set_dir_speed_whole;// 旋转速度
	/*舵机*/
	// 旋转角度
	int16_t cur_duoji_1_angle;// 一维平台旋转角度，基于正前方角度,左边为正，右边为负-135~135
	int16_t cur_duoji_2_angle;// 二维平台旋转角度，基于正前方角度,上面为负，下面为正-90~90
	
	/*摄像头*/
	uint8_t camera_mode;  			// 当前摄像头模式，实时视频/拍照
	uint8_t camera_workmode; 		// 当前摄像头工作模式，0:rgb565,1:jpeg  
	uint8_t camera_contrast;  	// 对比度等级
	uint8_t camera_saturation;  // 饱和度等级 
	uint8_t camera_effect;  		// 各种特效效果
	uint8_t camera_autoexposure_level;// 自动曝光等级
	uint8_t camera_light_mode;	// 白平衡模式 
	uint8_t camera_brightness;	// 亮度设置
	uint8_t camera_color_bar;		// 彩条设置
	uint8_t camera_size;     	 	// 摄像头输出图片尺寸
	uint8_t camera_is_abort;		// 中途退出摄像头采集标志
	
	/* 其他标志 */
	uint8_t camera_frame;// 帧率
	uint8_t car_control_transmission_state;// 小车连接控制状态
	uint8_t car_number;// 小车编号
	char car_secret[24];// 设备密钥
	uint8_t reset_wifi2;// 重置wifi2
}CAR;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern uint8_t ENTRY_NETWORK_METHOD;
extern CAR car;
extern uint8_t p;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void update_state(void);
void HAL_DCMI_XferHalfCpltCallback(DMA_HandleTypeDef *hdma);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY0_NETWORK_0_Pin GPIO_PIN_4
#define KEY0_NETWORK_0_GPIO_Port GPIOE
#define WIFI1_RESET_Pin GPIO_PIN_4
#define WIFI1_RESET_GPIO_Port GPIOF
#define PWM_RH_Pin GPIO_PIN_8
#define PWM_RH_GPIO_Port GPIOF
#define LED9_Pin GPIO_PIN_9
#define LED9_GPIO_Port GPIOF
#define LED10_Pin GPIO_PIN_10
#define LED10_GPIO_Port GPIOF
#define RH_CTRL2_Pin GPIO_PIN_1
#define RH_CTRL2_GPIO_Port GPIOC
#define KEY1_NETWORK_1_Pin GPIO_PIN_0
#define KEY1_NETWORK_1_GPIO_Port GPIOA
#define ENCODE_22_Pin GPIO_PIN_1
#define ENCODE_22_GPIO_Port GPIOA
#define PWM_DJ1_Pin GPIO_PIN_2
#define PWM_DJ1_GPIO_Port GPIOA
#define PWM_DJ2_Pin GPIO_PIN_3
#define PWM_DJ2_GPIO_Port GPIOA
#define PWM_LQ_Pin GPIO_PIN_5
#define PWM_LQ_GPIO_Port GPIOA
#define ENCODE_32_Pin GPIO_PIN_7
#define ENCODE_32_GPIO_Port GPIOA
#define PWM_RQ_Pin GPIO_PIN_0
#define PWM_RQ_GPIO_Port GPIOB
#define PWM_LH_Pin GPIO_PIN_1
#define PWM_LH_GPIO_Port GPIOB
#define ENCODE_11_Pin GPIO_PIN_9
#define ENCODE_11_GPIO_Port GPIOE
#define ENCODE_12_Pin GPIO_PIN_11
#define ENCODE_12_GPIO_Port GPIOE
#define LH_CTRL2_Pin GPIO_PIN_10
#define LH_CTRL2_GPIO_Port GPIOB
#define WIFI_TX_Pin GPIO_PIN_8
#define WIFI_TX_GPIO_Port GPIOD
#define WIFI_RX_Pin GPIO_PIN_9
#define WIFI_RX_GPIO_Port GPIOD
#define ENCODE_41_Pin GPIO_PIN_12
#define ENCODE_41_GPIO_Port GPIOD
#define ENCODE_42_Pin GPIO_PIN_13
#define ENCODE_42_GPIO_Port GPIOD
#define PF_T_Pin GPIO_PIN_9
#define PF_T_GPIO_Port GPIOA
#define PF_R_Pin GPIO_PIN_10
#define PF_R_GPIO_Port GPIOA
#define ENCODE_21_Pin GPIO_PIN_15
#define ENCODE_21_GPIO_Port GPIOA
#define WIFI2_RESET_Pin GPIO_PIN_10
#define WIFI2_RESET_GPIO_Port GPIOC
#define IMG_OUT_Pin GPIO_PIN_12
#define IMG_OUT_GPIO_Port GPIOC
#define LQ_CRTL1_Pin GPIO_PIN_0
#define LQ_CRTL1_GPIO_Port GPIOD
#define IMG_IN_Pin GPIO_PIN_2
#define IMG_IN_GPIO_Port GPIOD
#define LQ_CRTL2_Pin GPIO_PIN_3
#define LQ_CRTL2_GPIO_Port GPIOD
#define RQ_CRTL1_Pin GPIO_PIN_5
#define RQ_CRTL1_GPIO_Port GPIOD
#define DCMI_SCL_Pin GPIO_PIN_6
#define DCMI_SCL_GPIO_Port GPIOD
#define DCMI_SDA_Pin GPIO_PIN_7
#define DCMI_SDA_GPIO_Port GPIOD
#define DCMI_PWDN_Pin GPIO_PIN_9
#define DCMI_PWDN_GPIO_Port GPIOG
#define RQ_CTRL2_Pin GPIO_PIN_10
#define RQ_CTRL2_GPIO_Port GPIOG
#define DCMI_RESET_Pin GPIO_PIN_15
#define DCMI_RESET_GPIO_Port GPIOG
#define LH_CTRL1_Pin GPIO_PIN_3
#define LH_CTRL1_GPIO_Port GPIOB
#define ENCODE_31_Pin GPIO_PIN_4
#define ENCODE_31_GPIO_Port GPIOB
#define RH_CRTL1_Pin GPIO_PIN_0
#define RH_CRTL1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
// 配网方式
#define SmartConfig		1
#define SoftAP				2
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
