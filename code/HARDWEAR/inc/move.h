#ifndef __MOVE_H
#define __MOVE_H

#include "encode.h"
void move_braking(uint16_t v);// 小车制动

void move_turn_left(uint16_t v);// 小车原地左转（逆时针）
void move_turn_right(uint16_t v);// 小车原地右转（顺时针）
void move_all_dir_fix(uint16_t v,uint16_t angle);// 全向移动，朝向不变
void Adjust_Speed(int16_t v1,int16_t v2,int16_t v3,int16_t v4);//调速
void update_move(void);

extern uint8_t is_braking;
#endif
