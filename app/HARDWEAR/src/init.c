
#include "init.h"
#include "tim.h"
#include "ov2640.h"
#include "at_log.h"
#include "usart.h"
#include "pwm.h"

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
void init_net_method()
{	
	Log_d("【select net start】");
	p=20;// 10次交替闪烁，单次闪烁中2次延时计算 4s
	while(--p&&ENTRY_NETWORK_METHOD==0)
	{
		flicker_led(2,2,0,0,100);
		// softAp配网,按下高电平
		if(HAL_GPIO_ReadPin(KEY0_NETWORK_0_GPIO_Port,KEY0_NETWORK_0_Pin)==GPIO_PIN_SET){
			ENTRY_NETWORK_METHOD = SmartConfig;
		}else if(HAL_GPIO_ReadPin(KEY1_NETWORK_1_GPIO_Port,KEY1_NETWORK_1_Pin)==GPIO_PIN_SET){
			// SmartConfig配网
			ENTRY_NETWORK_METHOD = SoftAP;
		}
	}
	p=0;
	Log_d("【select net end】");
}


/* 定时器初始化 */

// 初始化基础定时器-编码器、测速
void init_base_encode()
{
	HAL_TIM_Base_Start_IT(&htim12);// 测速定时器
	
	HAL_TIM_Base_Start(&htim1);// 编码器
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start(&htim4);
}


// 初始化基础定时器-电机pwm
void init_base_move_pwm()
{
	HAL_TIM_Base_Start(&htim8);// 电机1-3pwm
	HAL_TIM_Base_Start(&htim13);// 电机4pwm
}

// 初始化基础定时器-舵机pwm
void init_base_duoji_pwm()
{
	update_duoji();// 初始化舵机位置，以免跑偏
	HAL_TIM_Base_Start(&htim5);// 舵机pwm
}

// 初始化基础定时器-其他基本定时器 us、串口5定时器
void init_base_other()
{
	HAL_TIM_Base_Start(&htim7);// us定时器
	HAL_TIM_Base_Start_IT(&htim11);
	HAL_TIM_Base_Stop_IT(&htim11);// 串口5定时器
}

// 初始化基础定时器-所有基础定时器
void init_base_all()
{
	Log_d("【init base start】");
	// 初始化基础定时器-编码器、测速
	init_base_encode();
	// 初始化基础定时器-电机pwm
	init_base_move_pwm();
	// 初始化基础定时器-舵机pwm
	init_base_duoji_pwm();
	// 初始化基础定时器-us
	init_base_other();
	HAL_Delay(200);
	//Log_d("【init base end】");
}

// 初始化ov2640模块
void init_ov2640()
{
	Log_d("【init ov2640 start】");
	while(OV2640_Init());
	Log_d("【init ov2640 end】");
}


// 初始化电机pwm使能通道
void init_enable_pwm_start_move()
{
	// 启动4个电机pwm中断，并使能通道
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim8,TIM_CHANNEL_1); // PA5
	HAL_TIMEx_PWMN_Start(&htim8,TIM_CHANNEL_2);	// PB0
	HAL_TIMEx_PWMN_Start(&htim8,TIM_CHANNEL_3);	// PB1
	HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_1);		// PF8
}

// 初始化舵机pwm使能通道
void init_enable_pwm_start_duoji()
{
	// 启动舵机pwm使能通道
	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_3);		// PA2
	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_4);		// PA3
}

// 初始化编码器使能通道
void init_enable_pwm_start_encode()
{
	// 启动编码器中断，并使能
	HAL_TIM_Encoder_Start(&htim1,TIM_CHANNEL_1);		// PE9
	HAL_TIM_Encoder_Start(&htim1,TIM_CHANNEL_2);		// PE11
	HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_1);		// PA1
	HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_2);		// PA15
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_1);		// PB4
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_2);		// PA7
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_1);		// PD12
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_2);		// PD13
}

// 初始化串口3使能接收中断
void init_enable_uart3_receive()
{
	// 必须先开启usart3接收中断
	HAL_UART_Receive_IT(&huart3,&p,1);
}

// 初始化所有使能通道、中断
void init_enable_channel_or_it()
{
	//Log_d("【init enbale channel or it start】");
	init_enable_pwm_start_move();// 初始化电机pwm使能通道
	init_enable_pwm_start_duoji();// 初始化舵机pwm使能通道
	init_enable_pwm_start_encode();// 初始化编码器使能通道
	init_enable_uart3_receive();// 初始化串口3使能接收中断
	//Log_d("【init enbale channel or it end】");
}


/* 取消定时器初始化 */

// 取消初始化基础定时器-编码器、测速
void deInit_base_encode()
{
	HAL_TIM_Base_Stop_IT(&htim12);// 测速定时器
	
	HAL_TIM_Base_Stop(&htim1);// 编码器
	HAL_TIM_Base_Stop(&htim2);
	HAL_TIM_Base_Stop(&htim3);
	HAL_TIM_Base_Stop(&htim4);
}


// 取消初始化基础定时器-电机pwm
void deInit_base_move_pwm()
{
	HAL_TIM_Base_Stop(&htim8);// 电机1-3pwm
	HAL_TIM_Base_Stop(&htim13);// 电机4pwm
}

// 取消初始化基础定时器-舵机pwm
void deInit_base_duoji_pwm()
{
	HAL_TIM_Base_Stop(&htim5);// 舵机pwm
}

// 取消初始化基础定时器-其他基本定时器 us、串口5定时器
void deInit_base_other()
{
	HAL_TIM_Base_Stop(&htim7);// us定时器
	HAL_TIM_Base_Stop(&htim11);// 串口5定时器
}

// 取消初始化基础定时器-所有基础定时器
void deInit_base_all()
{
	Log_d("【deInit base start】");
	// 取消初始化基础定时器-编码器、测速
	deInit_base_encode();
	// 取消初始化基础定时器-电机pwm
	deInit_base_move_pwm();
	// 取消初始化基础定时器-舵机pwm
	deInit_base_duoji_pwm();
	// 取消初始化基础定时器-us
	deInit_base_other();
	Log_d("【deInit base end】");
}


// 取消初始化电机pwm使能通道
void deInit_enable_pwm_start_move()
{
	// 启动4个电机pwm中断，并使能通道
	HAL_TIM_PWM_Stop(&htim8,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim8,TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim8,TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Stop(&htim8,TIM_CHANNEL_1); // PA5
	HAL_TIMEx_PWMN_Stop(&htim8,TIM_CHANNEL_2);	// PB0
	HAL_TIMEx_PWMN_Stop(&htim8,TIM_CHANNEL_3);	// PB1
	HAL_TIM_PWM_Stop(&htim13,TIM_CHANNEL_1);		// PF8
}

// 取消初始化舵机pwm使能通道
void deInit_enable_pwm_start_duoji()
{
	// 启动舵机pwm使能通道
	HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_3);		// PA2
	HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_4);		// PA3
}

// 取消初始化编码器使能通道
void deInit_enable_pwm_start_encode()
{
	// 启动编码器中断，并使能
	HAL_TIM_Encoder_Stop(&htim1,TIM_CHANNEL_1);		// PE9
	HAL_TIM_Encoder_Stop(&htim1,TIM_CHANNEL_2);		// PE11
	HAL_TIM_Encoder_Stop(&htim2,TIM_CHANNEL_1);		// PA1
	HAL_TIM_Encoder_Stop(&htim2,TIM_CHANNEL_2);		// PA15
	HAL_TIM_Encoder_Stop(&htim3,TIM_CHANNEL_1);		// PB4
	HAL_TIM_Encoder_Stop(&htim3,TIM_CHANNEL_2);		// PA7
	HAL_TIM_Encoder_Stop(&htim4,TIM_CHANNEL_1);		// PD12
	HAL_TIM_Encoder_Stop(&htim4,TIM_CHANNEL_2);		// PD13
}

// 取消初始化串口3使能接收中断
void deInit_enable_uart3_receive()
{
	HAL_UART_Abort_IT(&huart3);
}

// 取消初始化所有使能通道、中断
void deInit_enable_channel_or_it()
{
	Log_d("【deInit enbale channel or it start】");
	deInit_enable_pwm_start_move();// 取消初始化电机pwm使能通道
	deInit_enable_pwm_start_duoji();// 取消初始化舵机pwm使能通道
	deInit_enable_pwm_start_encode();// 取消初始化编码器使能通道
	deInit_enable_uart3_receive();// 取消初始化串口3使能接收中断
	Log_d("【deInit enbale channel or it end】");
}
