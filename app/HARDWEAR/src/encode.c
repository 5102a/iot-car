
#include "encode.h"
#include "math.h"
#include "move.h"
#include "at_log.h"


uint16_t Previous_Count[4] = {0,0,0,0};// 各轮上一次计数值

TIM_TypeDef* count_timer[4] = {TIM1,TIM2,TIM3,TIM4};

// 临时变量
uint16_t Current_Count[4]={0,0,0,0};// 当前计数值
uint8_t Current_Direction[4]={0,0,0,0};
uint16_t Current_Speed[4]={0.0,0.0,0.0,0.0};

// 每秒计数*0.01867 = 速度
// 外圈数*30.458 = 速度
//float K=1.0f/12.0f/4.0f/0.01f/34.0f*9.7f*3.14f;// 计数值*K = speed cm/s  1.8672456
double K=0.1867;// 计数值*K = speed cm/s  0.1s系数

uint8_t dir =0;
uint16_t currun = 0;
uint16_t count =0;
uint8_t i;
uint16_t tmp=800;
double new_count;

#define His_Len 5
int16_t His_Out1[His_Len],His_Out2[His_Len],His_Out3[His_Len],His_Out4[His_Len];   		//历史系统输出值
int16_t His_Val1[His_Len],His_Val2[His_Len],His_Val3[His_Len],His_Val4[His_Len];			//历史采样值（外部更新）
int16_t His_Err1[His_Len],His_Err2[His_Len],His_Err3[His_Len],His_Err4[His_Len];			//历史偏差
                     
PID pid[4] ={                
	{0,0,0,0,0,0.2,0.6,0.2,His_Out1,His_Val1,His_Err1,His_Len,180,3},
	{0,0,0,0,0,0.2,0.6,0.2,His_Out2,His_Val2,His_Err2,His_Len,180,3},
	{0,0,0,0,0,0.2,0.6,0.2,His_Out3,His_Val3,His_Err3,His_Len,180,3},
	{0,0,0,0,0,0.2,0.6,0.2,His_Out4,His_Val4,His_Err4,His_Len,180,3}
};

// 获取对应电机一段时间的计数值，采样周期取决于TIM12测速定时器周期 0.1s
uint16_t Read_Encoder_count(uint8_t pos)
{
  uint16_t Count,Current_Count;         

	// 保存当前计数值
  Current_Count=count_timer[pos]->CNT;

	if((count_timer[pos]->CR1&0x0010) == 0x0010) //如果反转                               
	{
		Current_Direction[pos]=1;//更新方向
		if(Previous_Count[pos] >= Current_Count){
			Count = Previous_Count[pos] - Current_Count;
		}else{
			// 反向溢出
			Count = 65536 - Current_Count + Previous_Count[pos];
		}
	}else{//如果正转
		Current_Direction[pos]=0;//更新方向
		if(Current_Count >= Previous_Count[pos]){
			Count = Current_Count - Previous_Count[pos];
		}else{
			// 正向溢出
			Count = 65536 - Previous_Count[pos] + Current_Count;
		}
	}          
	//Log_d("Count:%d",Count);
  Previous_Count[pos]=Current_Count;// 作为前一个计数值
	return Count>60000?(65536-Count):Count;// 抖动溢出矫正
}


// 获取各轮的pwm新计数值
uint16_t Get_New_Count(uint8_t pos)
{
	int16_t tmpVal;
	
	// 制动效果
	if(is_braking)
	{
		// 200ms制动效果
		if(count>=3) is_braking=0;// 取消制动，恢复pid计算
		count++;
		tmpVal=3000;// 制动电压
	}else{
		count=0;
		tmpVal = Get_New_Value(&pid[pos]); // 获取pid计算出的设定速度 cm/s
	}
	
	//Log_d("pos:%d-%d",pos,tmpVal);
	new_count = (double)tmpVal/18.67;// 实际测速计数值0-9,设定速度转换成对应计数值/1000

	// 实际测速计数值换算成对应的pwm计数值，拟合计数值
	new_count=141.1+593.7538*new_count-424.61377*pow(new_count,2)+142.173195*pow(new_count,3)-20.423*pow(new_count,4)+1.0779*pow(new_count,5);
	//Log_d("res:%f",new_count);
	
	// 计数值边界判断
	if(new_count<300){// 无法启动
		return 0;
	}else if(new_count>3999){// 超出pwm最大设定值
		return 3999;
	}else{
		return (uint16_t)new_count;
	}
}
// 更新当前所有轮的实际速度
void Actual_Speed(void)
{
	//Log_d("-----------------------------------------------");
	// 100ms获取一次电机计数值，并更新
	for(i=0;i<4;i++)
	{
		// 获取单个测速采样周期的计数值
		Current_Count[i]=Read_Encoder_count(i);
		//Log_d("Current_Count[1]:%d",Current_Count[i]);
		
		// 实际速度，cm/s
		new_count = ((double)Current_Count[i])*K;
		
		// 存储当前速度
		Current_Speed[i]=(uint16_t)(new_count+0.5);
		
		// 存储当前速度采样值
		pid[i].His_Val[pid[i].Cur_Index] = Current_Speed[i];
	}
	

	// 更新速度数据
	car.cur_speed_lq = Current_Direction[0]?-Current_Speed[0]:Current_Speed[0];
	car.cur_speed_rq = Current_Direction[1]?-Current_Speed[1]:Current_Speed[1];
	car.cur_speed_lh = Current_Direction[2]?-Current_Speed[2]:Current_Speed[2];
	car.cur_speed_rh = Current_Direction[3]?-Current_Speed[3]:Current_Speed[3];

	// 计算总体速度,不改变角度时，cm/s
	car.cur_speed_whole = (uint16_t)((Current_Speed[0]+Current_Speed[1]+Current_Speed[2]+Current_Speed[3])/4);
	car.cur_speed_angle = car.set_speed_angle;
	
	// 更新pwm设定值
	TIM8->CCR1 = Get_New_Count(0);
	TIM8->CCR2 = Get_New_Count(1);
	TIM8->CCR3 = Get_New_Count(2);
	TIM13->CCR1 = Get_New_Count(3);
	//Log_d("TIM-COUNT:%d,%d,%d,%d",TIM8->CCR1,TIM8->CCR2,TIM8->CCR3,TIM13->CCR1);
}
