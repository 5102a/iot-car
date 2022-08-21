#ifndef __SCCB_H
#define __SCCB_H
#include "main.h"

//IO方向设置
#define SCCB_SDA_IN()  {GPIOD->MODER&=~(3<<(7*2));GPIOD->MODER|=0<<7*2;}	//PD7 输入
#define SCCB_SDA_OUT() {GPIOD->MODER&=~(3<<(7*2));GPIOD->MODER|=1<<7*2;} 	//PD7 输出


//IO操作函数	 
#define SCCB_SCL(x)    	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,x)	 	//SCL
#define SCCB_SDA(x)    	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,x) 		//SDA	 

#define SCCB_READ_SDA    	HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_7)  		//输入SDA    
#define SCCB_ID   			0X60  			//OV2640的ID

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);
#endif













