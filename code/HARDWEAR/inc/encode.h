#ifndef __ENCODE_H
#define __ENCODE_H

#include "pid.h"

extern PID pid[4];

uint16_t Get_New_Count(uint8_t pos);
void Actual_Speed(void);
uint16_t Read_Encoder_count(uint8_t pos);

#endif
