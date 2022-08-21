
#include "move.h"
#include "math.h"
#include "at_log.h"

// 4电机控制逻辑io
//      左  右        左  右
// 前进 LQ1 RQ1  后退 LQ1 RQ1 前轮
//      LH2 RH2       LH2 RH2 后轮
// 定义4个电机方向
#define LH1(x) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,x) // 左后轮 PB3  PB10
#define LH2(x) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,x)
#define LQ1(x) HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,x) // 左前轮 PD0  PD3
#define LQ2(x) HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,x)
#define RQ1(x) HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,x) // 右前轮 PD5  PG10
#define RQ2(x) HAL_GPIO_WritePin(GPIOG,GPIO_PIN_10,x)
#define RH1(x) HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0,x) // 右后轮 PE0  PC1
#define RH2(x) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,x)

// L298N
// EN  IN1  IN2  行为
// 0    x    x   停止
// 1    0    0   制动
// 1    0    1   前进
// 1    1    0   后退
// 1    1    1   制动

uint8_t is_braking=0;

// 小车制动
void move_braking(uint16_t v)
{
	is_braking=1;
	LQ1(GPIO_PIN_RESET);// 左前轮
	LQ2(GPIO_PIN_RESET);
	
	RQ1(GPIO_PIN_RESET);// 右前轮
	RQ2(GPIO_PIN_RESET);
	
	LH1(GPIO_PIN_RESET);// 左后轮
	LH2(GPIO_PIN_RESET);
	
	RH1(GPIO_PIN_RESET);// 右后轮
	RH2(GPIO_PIN_RESET);
	Adjust_Speed(v,v,v,v);
}


// 小车原地左转（逆时针）
void move_turn_left(uint16_t v)
{
	LQ1(GPIO_PIN_SET);// 左前轮
	LQ2(GPIO_PIN_RESET);
	
	RQ1(GPIO_PIN_RESET);// 右前轮
	RQ2(GPIO_PIN_SET);
	
	LH1(GPIO_PIN_SET);// 左后轮
	LH2(GPIO_PIN_RESET);
	
	RH1(GPIO_PIN_RESET);// 右后轮
	RH2(GPIO_PIN_SET);
	Adjust_Speed(v,v,v,v);
}

// 小车原地右转（顺时针）
void move_turn_right(uint16_t v)
{
	LQ1(GPIO_PIN_RESET);// 左前轮
	LQ2(GPIO_PIN_SET);
	
	RQ1(GPIO_PIN_SET);// 右前轮
	RQ2(GPIO_PIN_RESET);
	
	LH1(GPIO_PIN_RESET);// 左后轮
	LH2(GPIO_PIN_SET);
	
	RH1(GPIO_PIN_SET);// 右后轮
	RH2(GPIO_PIN_RESET);
	Adjust_Speed(v,v,v,v);
}


// 固定朝向，任意角度平移
// 移动速度v cm，移动方向与前进方向的逆时针夹角angle 度
// 各轮子速度  左  右
//             v1  v2  前轮
//             v3  v4  后轮
// 小车全向移动
void move_all_dir_fix(uint16_t v,uint16_t angle)
{
	int16_t v1,v2,v3,v4;
	float cosVal,sinVal;
	cosVal = cosf(angle*0.017453);
	sinVal = sinf(angle*0.017453);
	v1 = v4 = (int16_t)(v*(cosVal - sinVal));
	v2 = v3 = (int16_t)(v*(cosVal + sinVal));
	
	// 左前轮
	if(v1>=0)
	{
		LQ1(GPIO_PIN_RESET);// 前进
		LQ2(GPIO_PIN_SET);
	}else{
		LQ1(GPIO_PIN_SET);// 后退
		LQ2(GPIO_PIN_RESET);
	}

	// 右前轮
	if(v2>=0)
	{
		RQ1(GPIO_PIN_RESET);// 前进
		RQ2(GPIO_PIN_SET);
	}else{
		RQ1(GPIO_PIN_SET);// 后退
		RQ2(GPIO_PIN_RESET);
	}
	
	// 左后轮
	if(v3>=0)
	{
		LH1(GPIO_PIN_RESET);// 前进
		LH2(GPIO_PIN_SET);
	}else{
		LH1(GPIO_PIN_SET);// 后退
		LH2(GPIO_PIN_RESET);
	}
	
	// 右后轮
	if(v4>=0)
	{
		RH1(GPIO_PIN_RESET);// 前进
		RH2(GPIO_PIN_SET);
	}else{
		RH1(GPIO_PIN_SET);// 后退
		RH2(GPIO_PIN_RESET);
	}
	Adjust_Speed(v1,v2,v3,v4);
}

// 调速，小车整体速度
void Adjust_Speed(int16_t v1,int16_t v2,int16_t v3,int16_t v4)
{
	//Log_d("speed:%d,%d,%d,%d",v1,v2,v3,v4);
	pid[0].Set_Val = v1 < 0 ? -v1 : v1;
	pid[1].Set_Val = v2 < 0 ? -v2 : v2;
	pid[2].Set_Val = v3 < 0 ? -v3 : v3;
	pid[3].Set_Val = v4 < 0 ? -v4 : v4;
}
// 更新小车移动
void update_move(void)
{
	is_braking=0;
	// 旋转
	if(car.set_speed_dir)
	{
		if(car.set_dir_speed_whole==0)
		{
			move_braking(0);// 制动
		}else{
			car.set_speed_dir==1?move_turn_left(car.set_dir_speed_whole):move_turn_right(car.set_dir_speed_whole);
		}
	}else{// 平移
		if(car.set_speed_whole==0)
		{
			move_braking(0);// 制动
		}else{
			move_all_dir_fix(car.set_speed_whole,car.set_speed_angle);
		}
	}
}

