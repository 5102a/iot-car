
#include "img.h"
#include "dcmi.h"
#include "ov2640.h"
#include "usart.h"
#include "esp8266.h"
#include "at_log.h"
#include "tim.h"
#include "iwdg.h"

uint8_t camera_mode=0;  			// 当前摄像头模式，实时视频/拍照
uint8_t camera_workmode=1; 		// 当前摄像头工作模式，0:rgb565,1:jpeg  
uint8_t camera_contrast=2;  	// 对比度等级
uint8_t camera_saturation=4;  // 饱和度等级 
uint8_t camera_effect=0;  		// 各种特效效果
uint8_t camera_autoexposure_level=0;// 自动曝光等级
uint8_t camera_light_mode=4;	// 白平衡模式 
uint8_t camera_brightness=3;	// 亮度设置
uint8_t camera_color_bar=0;		// 彩条设置
uint8_t camera_size=4;     	 	// 摄像头输出图片尺寸，默认是QVGA 320*240尺寸
uint8_t camera_is_abort=1;		//中途退出摄像头采集标志


__align(4) uint32_t img_buf[img_buf_size];	//数据缓存buf

volatile uint32_t img_data_len=0; 			//buf中的JPEG有效数据长度 
volatile uint8_t img_data_ok=0;					//JPEG数据采集完成标志 
uint8_t is_start=0;// 摄像头启动标志位


//JPEG尺寸支持列表
const uint16_t jpeg_img_size_tbl[][2]=
{
	// 91260/x = fps
	160,120,	//QQVGA 0 512=512*4=2048
	176,144,	//QCIF 1 768=768*4=3072     15
	320,240,	//QVGA 2 1536=1536*4=6144   14.8
	400,240,	//WQVGA 3 1792=1792*4=7168  12.7
	352,288,	//CIF 4 2048 = 2048*4=8192   11.1
	640,480,	//VGA 5 4096 = 4096*4=16384  5.6
	800,600,	//SVGA 6 5888=5888*4=23552   3.8
	1024,768,	//XGA  7 8704=8704*4=34816   2.6
	1280,800,	//WXGA 8 10752=10752*4=43008  2.1
	1280,960,	//XVGA 9 12544=12544*4=50176
	1440,900,	//WXGA+ 10 18176=72704
	1280,1024,	//SXGA	11 14080=56320
	1600,1200,	//UXGA	12 20480=81920
};


// 设置相机
void set_camera_config(void)
{
	// 修改对比度
	if(car.camera_contrast!=camera_contrast) 
	{
		camera_contrast = car.camera_contrast;
		OV2640_Contrast(camera_contrast);
	}
	// 修改饱和度
	if(car.camera_saturation!=camera_saturation) 
	{
		camera_saturation = car.camera_saturation;
		OV2640_Color_Saturation(camera_saturation);
	}
	// 修改特效
	if(car.camera_effect!=camera_effect) 
	{
		camera_effect = car.camera_effect;
		OV2640_Special_Effects(camera_effect);
	}
	// 修改曝光等级
	if(car.camera_autoexposure_level!=camera_autoexposure_level) 
	{
		camera_autoexposure_level = car.camera_autoexposure_level;
		OV2640_Auto_Exposure(camera_autoexposure_level);
	}
	// 修改白平衡
	if(car.camera_light_mode!=camera_light_mode) 
	{
		camera_light_mode = car.camera_light_mode;
		OV2640_Light_Mode(camera_light_mode);
	}
	// 修改亮度
	if(car.camera_brightness!=camera_brightness) 
	{
		camera_brightness = car.camera_brightness;
		OV2640_Brightness(camera_brightness);
	}
	// 修改彩条设置
	if(car.camera_color_bar!=camera_color_bar) 
	{
		camera_color_bar = car.camera_color_bar;
		OV2640_Color_Bar(camera_color_bar);
	}
	// 修改图像尺寸大小
	if(car.camera_size!=camera_size) 
	{
		camera_size = car.camera_size;
		// 设置输出图像尺寸
		OV2640_OutSize_Set(jpeg_img_size_tbl[car.camera_size][0],jpeg_img_size_tbl[car.camera_size][1]);
	}
}

// 发送图像数据到串口
void send_img_data(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	uint8_t i=0;
	uint8_t min = 1;// 最少传输次数
	uint16_t res;
	if(Size>=16384)//512 16384
	{
		while((Size/++min)>=16384);// 求出串口最少传输次数
	}
	res = Size/min*4 + 1;
	while(i<=min)
	{
		while(HAL_UART_Transmit_DMA(huart,pData+i*res,res));
		i++;
	}
	HAL_UART_AbortTransmit_IT(huart);
}


// 初始化摄像头
void init_camera()
{
	Log_d("【init camera start】");

	HAL_TIM_Base_Start_IT(&htim6);// 启动帧率中断定时器
	
	if(car.camera_workmode)
		OV2640_JPEG_Mode();		// 开启ov2640 jpeg模式
	else
		OV2640_RGB565_Mode(); // ov2640启动RGB565模式
	
	// 设置输出图像尺寸
	OV2640_OutSize_Set(jpeg_img_size_tbl[car.camera_size][0],jpeg_img_size_tbl[car.camera_size][1]);
	Log_d("【init OV2640_OutSize_Set success】");

	// 设置采集模式,开启了双缓冲
	HAL_DCMI_Start_DMA(&hdcmi,(car.camera_mode?DCMI_MODE_SNAPSHOT:DCMI_MODE_CONTINUOUS),(uint32_t)&img_buf, img_buf_size);
	is_start=1;// 标记摄像头初始化完成
	Log_d("【init camera end】");
}

// 停止摄像头
void deInit_camera()
{
	Log_d("【deInit camera start】");
	
	HAL_UART_DMAStop(&huart5);// 停止串口5DMA传输
	
	__HAL_DCMI_DISABLE_IT(&hdcmi, DCMI_IT_FRAME);
	
	HAL_TIM_Base_Stop_IT(&htim6);// 停止帧率计数中断
	
	// esp8266休眠

	HAL_DCMI_Stop(&hdcmi);

	car.camera_frame=0;
	img_data_ok=0;
	is_start=0;
	UART5_RX_STA=0;
	Log_d("【deInit camera end】");
}

// 停止捕获并上传图片数据
void suspend_and_send_imgdata()
{
	HAL_DCMI_Suspend(&hdcmi);// 暂停捕获图像
	__HAL_DMA_DISABLE(&hdma_dcmi);//关闭DMA
	while(DMA2_Stream1->CR&0X01);	//等待DMA2_Stream1可配置   
	// 获取本次采集的一帧数据长度，必须小于buf
	img_data_len=img_buf_size-__HAL_DMA_GET_COUNTER(&hdma_dcmi);
	//Log_d("%d",img_data_len);
	// 传送图像数据
	send_img_data(&huart5,(uint8_t *)img_buf,img_data_len);
}

// 恢复捕获并开始下一轮采集
void resume_and_start_next()
{
	__HAL_DMA_SET_COUNTER(&hdma_dcmi,img_buf_size);// 重置计数
	__HAL_DMA_ENABLE(&hdma_dcmi); //打开DMA
	__HAL_DCMI_ENABLE_IT(&hdcmi,DCMI_IT_FRAME);// 开启下一帧中断
	HAL_DCMI_Resume(&hdcmi);// 恢复捕获
	img_data_ok=0;// 标记新一轮采集开始
}

//JPEG模式采集图像
void collect_image_data(void)
{
	Log_d("【collect_image_data start】");
	while(1)
	{
		// 帧中断中会置为1
		if(img_data_ok==1)	//jpeg数据采集完成到buf中，准备启动buf->uart5的dma传输
		{
//			// 停止捕获并上传图片数据
			suspend_and_send_imgdata();
			//HAL_IWDG_Refresh(&hiwdg);
			//Log_d("%d  %d",img_buf_size*4 ,img_data_len*4);
			
			// 恢复捕获并开始下一轮采集
			resume_and_start_next();
		}

	}    
} 

