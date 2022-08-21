#ifndef __INIT_H
#define __INIT_H
#include "main.h"

// led灯单次闪烁
void flicker_led(uint8_t group,uint8_t flicker,uint8_t sync,uint8_t keep,uint16_t delay);
// 初始化选择入网方式
void init_net_method(void);
// 初始化基础定时器-编码器、测速
void init_base_encode(void);
	// 初始化基础定时器-电机pwm
void init_base_move_pwm(void);
	// 初始化基础定时器-舵机pwm
void init_base_duoji_pwm(void);
// 初始化基础定时器-其他基本定时器 us
void init_base_other(void);

// 初始化基础定时器-所有基础定时器
void init_base_all(void);

// 初始化ov2640模块
void init_ov2640(void);
// 初始化电机pwm使能通道
void init_enable_pwm_start_move(void);
// 初始化舵机pwm使能通道
void init_enable_pwm_start_duoji(void);
// 初始化编码器使能通道
void init_enable_pwm_start_encode(void);
// 初始化串口3使能接收中断
void init_enable_uart3_receive(void);

// 初始化所有使能通道、中断
void init_enable_channel_or_it(void);



// 取消初始化基础定时器-编码器、测速
void deInit_base_encode(void);
// 取消初始化基础定时器-电机pwm
void deInit_base_move_pwm(void);
// 取消初始化基础定时器-舵机pwm
void deInit_base_duoji_pwm(void);	
// 取消初始化基础定时器-其他基本定时器 us
void deInit_base_other(void);

// 取消初始化基础定时器-所有基础定时器
void deInit_base_all(void);

// 取消初始化电机pwm使能通道
void deInit_enable_pwm_start_move(void);
// 取消初始化舵机pwm使能通道
void deInit_enable_pwm_start_duoji(void);
// 取消初始化编码器使能通道
void deInit_enable_pwm_start_encode(void);
// 取消初始化串口3使能接收中断
void deInit_enable_uart3_receive(void);

// 取消初始化所有使能通道、中断
void deInit_enable_channel_or_it(void);



#endif


