#ifndef __PWM_H
#define __PWM_H

#include "main.h"

typedef struct
{
	uint16_t Degree_Count;
	int16_t Set_Degree;
}Dj_Degree;

extern Dj_Degree dj_degree[2];

uint16_t Get_Degree_Count(uint8_t pos);
void Set_1_degree(int16_t degree);
void Set_2_degree(int16_t degree);
void update_duoji(void);

#endif
