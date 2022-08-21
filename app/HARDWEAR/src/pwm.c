
#include "pwm.h"
#include "tim.h"
#include "at_log.h"

// 舵机pwm初始值
Dj_Degree dj_degree[2]={
	{1500,135},// 一维
	{2000,135},// 二维
};


// 获取各平台角度对应的计数值
uint16_t Get_Degree_Count(uint8_t pos)
{
	// 二维
	if(pos)
		return (uint16_t)((0.5 + 0.01111*dj_degree[1].Set_Degree)*1000);
	else 
		return (uint16_t)((0.493 + 0.0074062*dj_degree[0].Set_Degree)*1000);
}

// 设置第一维平台舵机角度
void Set_1_degree(int16_t degree)
{
	// 0.5==0 1 == 70 1.5ms == 135 , 2.0==205  ,2.5 == 270 ,3==70
	// count = 0.493 + 0.0074062*deg
	dj_degree[0].Set_Degree = degree + 135;
	dj_degree[0].Degree_Count = Get_Degree_Count(0);// 获取角度对应的pwm计数值
	
	// 设置舵机pwm设定值
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3, dj_degree[0].Degree_Count);
	// 更新舵机旋转角度
	car.cur_duoji_1_angle = dj_degree[0].Set_Degree - 135;
}

// 设置第二维平台舵机角度
void Set_2_degree(int16_t degree)
{
	// 0.5==0, 1==45,1.5==90,2.0==135,2.5==180
	// count = 0.5 + 0.01111*deg
	// 舵机最大角，防止过头损坏
	degree = degree < -45? -45 : degree;
	dj_degree[1].Set_Degree = degree + 90;
	dj_degree[1].Degree_Count = Get_Degree_Count(1);// 获取角度对应的pwm计数值
	
	// 设置舵机pwm设定值
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, dj_degree[1].Degree_Count);
	// 更新舵机旋转角度
	car.cur_duoji_2_angle = dj_degree[1].Set_Degree - 90;
}

// 更新舵机角度
void update_duoji(void)
{
	// 更新舵机1
	Set_1_degree(car.cur_duoji_1_angle);
	// 更新舵机2
	Set_2_degree(car.cur_duoji_2_angle);
}

