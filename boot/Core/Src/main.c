/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "esp8266.h"
#include "iap.h"
#include "stmflash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t has_receive_app=0;
uint8_t auto_entry_app=1;
uint8_t p=0;
uint8_t ENTRY_NETWORK_METHOD = 0;// 默认配网
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 单次灯闪烁 0 led9,1 led10,2 都有,flicker是否闪烁，0灭，1亮，2闪一次,sync 是否同时闪烁双闪时有效
// keep 最终保持的状态，0熄灭，1亮，2原本状态,delay延迟ms
void flicker_led(uint8_t group,uint8_t flicker,uint8_t sync,uint8_t keep,uint16_t delay)
{
	// 只亮
	if(flicker==1||flicker==2){
		switch(group){
			case 2:
				if(sync){
					HAL_GPIO_WritePin(LED9_GPIO_Port,LED9_Pin,GPIO_PIN_RESET);
					HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,GPIO_PIN_RESET);
				}else{
					HAL_GPIO_WritePin(LED9_GPIO_Port,LED9_Pin,GPIO_PIN_RESET);
					HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,GPIO_PIN_SET);
				}
				break;
			case 1:
				HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,GPIO_PIN_RESET);break;
			case 0:
				HAL_GPIO_WritePin(LED9_GPIO_Port,LED9_Pin,GPIO_PIN_RESET);break;
			default:;
		}
		HAL_Delay(delay);
	}
	// 只灭
	if(flicker==0||flicker==2){
		switch(group){
			case 2:
				if(sync){
					HAL_GPIO_WritePin(LED9_GPIO_Port,LED9_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,GPIO_PIN_SET);
				}else{
					HAL_GPIO_WritePin(LED9_GPIO_Port,LED9_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,GPIO_PIN_RESET);
				}
				break;
			case 1:
				HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,GPIO_PIN_SET);break;
			case 0:
				HAL_GPIO_WritePin(LED9_GPIO_Port,LED9_Pin,GPIO_PIN_SET);break;
			default:;
		}
		HAL_Delay(delay);
	}
	switch(keep){
		case 1:
			HAL_GPIO_WritePin(LED9_GPIO_Port,LED9_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,GPIO_PIN_RESET);
			break;
		case 0:
			HAL_GPIO_WritePin(LED9_GPIO_Port,LED9_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED10_GPIO_Port,LED10_Pin,GPIO_PIN_SET);
			break;
		default:;
	}
}

// 初始化选择入网方式
void init_net_method(void)
{	
	printf("【select net start】\r\n");
	p=20;// 10次交替闪烁，单次闪烁中2次延时计算 4s
	while(--p&&ENTRY_NETWORK_METHOD==0)
	{
		flicker_led(2,2,0,0,100);
		// smartCOnfig配网,按下高电平
		if(HAL_GPIO_ReadPin(KEY0_NETWORK_0_GPIO_Port,KEY0_NETWORK_0_Pin)==GPIO_PIN_SET){
			ENTRY_NETWORK_METHOD = SmartConfig;
		}
	}
	p=0;
	printf("【select net end】\r\n");
}

// 进入应用程序
void entry_app(void)
{
	printf("开始执行FLASH用户代码!!\r\n");
	if(((*(__IO uint32_t *)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
	{	 
		flicker_led(2,2,0,0,300);
		flicker_led(2,2,0,0,300);
		HAL_Delay(1000);
		HAL_IWDG_Refresh(&hiwdg);
		HAL_UART_Abort_IT(&huart1);// 终止串口1
		HAL_UART_Abort_IT(&huart5);// 终止串口5
		HAL_TIM_Base_MspDeInit(&htim14);// 取消定时器
		HAL_RCC_DeInit(); // 关闭外设
		iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
	}else{
		printf("非FLASH应用程序,无法执行!\r\n");  
	}	
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint32_t oldcount=0;
	uint32_t applenth=0;
	uint8_t i=0;
	uint8_t * car_data="{\"car_secret\":\"VlzbGfFuvRV8o67sTlRLMw==\"}";
	char *p;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART5_Init();
  MX_TIM14_Init();
  MX_USART1_UART_Init();
  MX_IWDG_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
	// 选择配网方式
	init_net_method();
	atk_8266_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		// 接收中
		if(UART5_RX_STA)
		{
			// 接收固件中
			if(has_receive_app){
				//新周期内,没有收到任何数据,认为本次数据接收完成.
				if(oldcount==UART5_RX_STA)
				{
					applenth=UART5_RX_STA;
					oldcount=0;
					UART5_RX_STA=0;
					printf("用户程序接收完成!\r\n");
					printf("代码长度:%dBytes\r\n",applenth);
					
					printf("开始更新固件...\r\n");	
					if(((*(__IO uint32_t*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
					{	 
						iap_write_appbin(FLASH_APP1_ADDR,UART5_RX_BUF,applenth);//更新FLASH代码 
						printf("固件更新完成!\r\n");	
						// 进入应用程序
						entry_app();	
					}else{ 
						printf("非FLASH应用程序!\r\n");
					}
					// 清除接收固件标志位
					has_receive_app=0;
				}else{
					// 接收固件中
					printf("已下载:%d\r\n",UART5_RX_STA);
					oldcount=UART5_RX_STA;			
				}
			}else if(UART5_RX_STA&0x80000000){
				// 指令接收完成
				// 接收指令中
				// system_reset: r-2
				// receive_app: r-3
				// entry_app: a-1
				// cancel_auto_app: c-0
				printf("%s\r\n",UART5_RX_BUF);
				// 系统复位，重新进入bootloader
				p = strstr((const char*)UART5_RX_BUF,"r-2");
				if(p) NVIC_SystemReset();// 系统复位
				
				// 准备开始接收应用程序
				p = strstr((const char*)UART5_RX_BUF,"r-3");
				if(p) has_receive_app=1;
				
				// 直接进入应用程序
				p = strstr((const char*)UART5_RX_BUF,"a-1");
				if(p) entry_app();
				
				// 取消自动进入应用程序
				p = strstr((const char*)UART5_RX_BUF,"c-0");
				if(p) auto_entry_app=0;
				
				UART5_RX_STA=0;
				HAL_UART_Receive_IT(&huart5,&UART5_RX_BUF[0],1);// 再次开启接收
			}

		}else{
			// 未接收到数据
			// 2s 发送一次设备密钥维持连接
			if(i%20==0){
				HAL_UART_Transmit_IT(&huart5,car_data,strlen((const char *)car_data));
				// 刷新看门狗
				HAL_IWDG_Refresh(&hiwdg);
				// 10s后自动进入应用程序
				if(i>=100){
					i=0;
					// 进入应用程序
					if(auto_entry_app) entry_app();		
				}
			}
		}

		i++;
		HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* TIM8_TRG_COM_TIM14_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
}

/* USER CODE BEGIN 4 */
// 串口接收中断
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// 接收未完成
	if(huart->Instance==UART5 && (UART5_RX_STA&0x80000000)==0)
	{		
		// 小车指令解析
		if(init_wifi2_finish)
		{	
			// 接收指令
			if(has_receive_app==0){
				if(UART5_RX_BUF[UART5_RX_STA]=='@'){
					UART5_RX_STA=0;
					HAL_UART_Receive_IT(&huart5,&UART5_RX_BUF[0],1);// 正常接收数据
				}else if(UART5_RX_BUF[UART5_RX_STA]=='$'){
					UART5_RX_BUF[UART5_RX_STA&0x7FFFFFFF]='\0';
					UART5_RX_STA|=0x80000000;	//标记接收完成
				}else{
					HAL_UART_Receive_IT(&huart5,&UART5_RX_BUF[++UART5_RX_STA],1);// 正常接收数据
				}
			}else{
				HAL_UART_Receive_IT(&huart5,&UART5_RX_BUF[++UART5_RX_STA],1);// 正常接收数据
			}
		}else{
			// wifi2初始化
			// 直到间隔50ms没数据则标记完成接收
			if(UART5_RX_STA==0) HAL_TIM_Base_Start_IT(&htim14);
			__HAL_TIM_SET_COUNTER(&htim14,0);
			HAL_UART_Receive_IT(&huart5,&UART5_RX_BUF[++UART5_RX_STA],1);// 开启下次接收
			if(UART5_RX_STA+1>=UART5_MAX_RECV_LEN) UART5_RX_STA=0;// 超出接收，重置接收
		}
	}
}

// 定时器中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */

  /* USER CODE BEGIN Callback 1 */

	// 串口5用到的定时器
	if(htim->Instance == TIM14)
	{
		HAL_TIM_Base_Stop_IT(&htim14);
		HAL_UART_AbortReceive_IT(&huart5);// 终止串口5接收中断
		UART5_RX_BUF[UART5_RX_STA&0x7FFFFFFF]='\0';
		UART5_RX_STA|=0x80000000;	//标记接收完成
		return;
	}

  /* USER CODE END Callback 1 */
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
