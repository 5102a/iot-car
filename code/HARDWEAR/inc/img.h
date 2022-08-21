#ifndef __IMG_H
#define __IMG_H
#include "main.h"

// 1024*21
#define img_buf_size 1024*16  			//定义数据缓存img_buf的大小(*4字节)

extern uint8_t camera_mode;  			// 当前摄像头模式，实时视频/拍照
extern uint8_t camera_workmode; 		// 当前摄像头工作模式，0:rgb565,1:jpeg  
extern uint8_t camera_contrast;  	// 对比度等级
extern uint8_t camera_saturation;  // 饱和度等级 
extern uint8_t camera_effect;  		// 各种特效效果
extern uint8_t camera_autoexposure_level;// 自动曝光等级
extern uint8_t camera_light_mode;	// 白平衡模式 
extern uint8_t camera_brightness;	// 亮度设置
extern uint8_t camera_color_bar;		// 彩条设置
extern uint8_t camera_size;     	 	// 摄像头输出图片尺寸，默认是QVGA 320*240尺寸
extern uint8_t camera_is_abort;		//中途退出摄像头采集标志

extern __align(4) uint32_t img_buf[img_buf_size]; 			//定义数据缓存img_buf
extern volatile uint32_t img_data_len; 			//buf中的JPEG有效数据长度 
extern volatile uint8_t img_data_ok;					//JPEG数据采集完成标志 
extern uint8_t is_start;

void set_camera_config(void);
void init_camera(void);
void deInit_camera(void);
void collect_image_data(void);
// 停止捕获并上传图片数据
void suspend_and_send_imgdata(void);
// 恢复捕获并开始下一轮采集
void resume_and_start_next(void);


#endif


