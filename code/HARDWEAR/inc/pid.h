#ifndef __PID_H
#define __PID_H

#include "main.h"

typedef struct
{
	uint16_t Cur_Index;        //当前值索引
	uint16_t Pre_Index;        //前一个索引
	uint16_t PPre_Index;       //前二个索引
	uint8_t Init_Count;        //增量初始化
	int16_t Set_Val;      	//设定要达到的值（外部提供值）
	float Kp;							//比例系数（外部提供值）
  float Ki;							//积分系数（外部提供值）
	float Kd;							//微分系数（外部提供值）
	int16_t *His_Out;   		//历史系统输出值
	int16_t *His_Val;				//历史采样值（外部更新）
	int16_t *His_Err;				//历史偏差
	uint16_t His_Len;        //历史数组长度
	int16_t Upper_Limit;      //上限
	int16_t Lower_Limit;      //下限
}PID;

int16_t Get_New_Value(PID * pid);


#endif
