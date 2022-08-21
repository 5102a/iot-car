/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "qcloud_iot_api_export.h"
#include "lite-utils.h"
#include "at_client.h"
#include "string.h"
#include "data_config.c"

#include "img.h"
#include "dcmi.h"
#include "ov2640.h"
#include "usart.h"
#include "esp8266.h"
#include "iwdg.h"
#include "move.h"
#include "init.h"


static bool sg_control_msg_arrived = false;
static char sg_data_report_buffer[AT_CMD_MAX_LEN];// 上报数据缓冲区
static size_t sg_data_report_buffersize = sizeof(sg_data_report_buffer) / sizeof(sg_data_report_buffer[0]);

/* 事件 */
#ifdef EVENT_POST_ENABLED
#include "events_config.c"
#ifdef	EVENT_TIMESTAMP_USED
// 更新事件 时间戳
static void update_events_timestamp(sEvent *pEvents, int count)
{
	int i;
	
	for(i = 0; i < count; i++){
        if (NULL == (&pEvents[i])) { 
	        Log_e("null event pointer"); 
	        return; 
        }
		pEvents[i].timestamp = HAL_GetTimeSeconds();
	}
}
#endif 
// 事件回复之后的回调，清除事件标志(回复服务器下发的事件指令)
static void event_post_cb(char *msg, void *context)
{
	Log_d("eventReply:%s", msg);
	IOT_Event_clearFlag(context, FLAG_EVENT0|FLAG_EVENT1|FLAG_EVENT2|FLAG_EVENT3);
}

#endif

/* 行为 */
#ifdef ACTION_ENABLED
#include "action_config.c"

// 服务器下发收到行为指令之后的回调，处理逻辑并返回执行结果响应给服务器
// action : regist action and set the action handle callback, add your aciton logic here
static void OnActionCallback(void *pClient, const char *pClientToken, DeviceAction *pAction) 
{	
	int i;
	sReplyPara replyPara;

	//do something base on input, just print as an sample
	DeviceProperty *pActionInput = pAction->pInput;
	for (i = 0; i < pAction->input_num; i++) {		
		if (JSTRING == pActionInput[i].type) {
			Log_d("Input:[%s], data:[%s]",  pActionInput[i].key, pActionInput[i].data);
			HAL_Free(pActionInput[i].data);
		} else {
			if(JINT32 == pActionInput[i].type) {
				Log_d("Input:[%s], data:[%d]",  pActionInput[i].key, *((int*)pActionInput[i].data));
			} else if( JFLOAT == pActionInput[i].type) {
				Log_d("Input:[%s], data:[%f]",  pActionInput[i].key, *((float*)pActionInput[i].data));
			} else if( JUINT32 == pActionInput[i].type) {
				Log_d("Input:[%s], data:[%u]",  pActionInput[i].key, *((uint32_t*)pActionInput[i].data));
			}
		}
	}	

	// construct output 
	memset((char *)&replyPara, 0, sizeof(sReplyPara));
	replyPara.code = eDEAL_SUCCESS;
	replyPara.timeout_ms = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;						
	strcpy(replyPara.status_msg, "action execute success!"); //add the message about the action resault 
	
	
	DeviceProperty *pActionOutnput = pAction->pOutput;	
	(void)pActionOutnput; //elimate warning
	//TO DO: add your aciont logic here and set output properties which will be reported by action_reply
	
	// 回复
	IOT_ACTION_REPLY(pClient, pClientToken, sg_data_report_buffer, sg_data_report_buffersize, pAction, &replyPara); 	
}

// 注册行为的数据模板，给所有行为添加回调函数
static int _register_data_template_action(void *pTemplate_client)
{
	int i,rc;
	
    for (i = 0; i < TOTAL_ACTION_COUNTS; i++) {
	    rc = IOT_Template_Register_Action(pTemplate_client, &g_actions[i], OnActionCallback);
	    if (rc != QCLOUD_RET_SUCCESS) {
	        rc = IOT_Template_Destroy(pTemplate_client);
	        Log_e("register device data template action failed, err: %d", rc);
	        return rc;
	    } else {
	        Log_i("data template action=%s registered.", g_actions[i].pActionId);
	    }
    }

	return QCLOUD_RET_SUCCESS;
}
#endif

// 控制命令的回调函数
static void OnControlMsgCallback(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty) 
{
    int i = 0;

    for (i = 0; i < TOTAL_PROPERTY_COUNT; i++) {		
        if (strcmp(sg_DataTemplate[i].data_property.key, pProperty->key) == 0) {
            sg_DataTemplate[i].state = eCHANGED;
            //Log_i("Property=%s changed", pProperty->key);
            sg_control_msg_arrived = true;
            return;
        }
    }

    Log_e("Property=%s changed no match", pProperty->key);
}

// 注册数据模板属性
static int _register_data_template_property(void *ptemplate_client)
{
	int i,rc;
	
    for (i = 0; i < TOTAL_PROPERTY_COUNT; i++) {
	    rc = IOT_Template_Register_Property(ptemplate_client, &sg_DataTemplate[i].data_property, OnControlMsgCallback);
	    if (rc != QCLOUD_RET_SUCCESS) {
	        rc = IOT_Template_Destroy(ptemplate_client);
	        Log_e("register device data template property failed, err: %d", rc);
	        return rc;
	    } else {
	        //Log_i("data template property=%s registered.", sg_DataTemplate[i].data_property.key);
	    }
    }

	return QCLOUD_RET_SUCCESS;
}

// 上报回复报告的回调函数
static void OnReportReplyCallback(void *pClient, Method method, ReplyAck replyAck, const char *pJsonDocument, void *pUserdata) {
	//Log_i("recv report_reply(ack=%d): %s", replyAck,pJsonDocument);
}


/*用户需要实现的下行数据的业务逻辑,待用户实现*/
static void deal_down_stream_user_logic(void *pClient, ProductDataDefine   * pData)
{
	// 更新设置
	car.set_speed_whole = pData->m_set_speed_whole;// 当前整体车速，cm/s
	car.set_speed_angle = pData->m_set_speed_angle;// 移动角度	
	car.set_speed_dir = pData->m_set_speed_dir;// 旋转方向
	car.set_dir_speed_whole = pData->m_set_dir_speed_whole;// 旋转速度
	car.cur_duoji_1_angle = -pData->m_cur_duoji_1_angle;// 一维平台旋转角度，基于正前方角度,左边为正，右边为负-135~135
	car.cur_duoji_2_angle = -pData->m_cur_duoji_2_angle+45;// 二维平台旋转角度，基于正前方角度,上面为负，下面为正-90~90
	car.camera_mode = pData->m_camera_mode;// 当前摄像头模式，实时视频/拍照
	car.camera_workmode = pData->m_camera_workmode;// 当前摄像头工作模式，0:rgb565,1:jpeg  
	car.camera_contrast = pData->m_camera_contrast;// 对比度等级
	car.camera_saturation = pData->m_camera_saturation;// 饱和度等级 
	car.camera_effect = pData->m_camera_effect;// 各种特效效果
	car.camera_autoexposure_level = pData->m_camera_autoexposure_level;// 自动曝光等级
	car.camera_light_mode = pData->m_camera_light_mode;// 白平衡模式 
	car.camera_brightness = pData->m_camera_brightness;// 亮度设置
	car.camera_color_bar = pData->m_camera_color_bar;// 彩条设置
	car.camera_size = pData->m_camera_size;// 摄像头输出图片尺寸
	car.camera_is_abort = pData->m_camera_is_abort;// 中途退出摄像头采集标志
	car.reset_wifi2 = pData->m_reset_wifi2;// 重置wifi2标志位
	//Log_d("【user data is update】");
}

/*用户需要实现的上行数据的业务逻辑,此处仅供示例*/
static int deal_up_stream_user_logic(void *pClient, DeviceProperty *pReportDataList[], int *pCount,ProductDataDefine   * pData)
{
	int i, j;

	// 更新设置
	pData->m_cur_speed_whole = car.cur_speed_whole;// 当前整体车速，cm/s
	pData->m_cur_speed_angle=car.cur_speed_angle ;// 移动角度	
	pData->m_set_speed_dir = car.set_speed_dir;// 当前移动方向
	pData->m_set_dir_speed_whole=car.set_dir_speed_whole ;// 旋转速度
	pData->m_cur_speed_lq=car.cur_speed_lq ;// 左前轮速度
	pData->m_cur_speed_rq=car.cur_speed_rq ;// 右前轮速度
	pData->m_cur_speed_lh=car.cur_speed_lh ;// 左后轮速度
	pData->m_cur_speed_rh=car.cur_speed_rh ;// 右后轮速度
	pData->m_cur_duoji_1_angle=-car.cur_duoji_1_angle ;// 一维平台旋转角度，基于正前方角度,左边为正，右边为负-135~135
	pData->m_cur_duoji_2_angle=-car.cur_duoji_2_angle+45 ;// 二维平台旋转角度，基于正前方角度,上面为负，下面为正-90~90
	pData->m_camera_mode=car.camera_mode ;// 当前摄像头模式，实时视频/拍照
	pData->m_camera_workmode=car.camera_workmode ;// 当前摄像头工作模式，0:rgb565,1:jpeg  
	pData->m_camera_contrast=car.camera_contrast ;// 对比度等级
	pData->m_camera_saturation=car.camera_saturation ;// 饱和度等级 
	pData->m_camera_effect=car.camera_effect ;// 各种特效效果
	pData->m_camera_autoexposure_level=car.camera_autoexposure_level ;// 自动曝光等级
	pData->m_camera_light_mode=car.camera_light_mode ;// 白平衡模式 
	pData->m_camera_brightness=car.camera_brightness ;// 亮度设置
	pData->m_camera_color_bar=car.camera_color_bar ;// 彩条设置
	pData->m_camera_size=car.camera_size ;// 摄像头输出图片尺寸
	pData->m_camera_is_abort=car.camera_is_abort ;// 中途退出摄像头采集标志
	pData->m_camera_frame=car.camera_frame ;// 帧率
	pData->m_car_control_transmission_state=car.car_control_transmission_state;// 小车连接控制状态
	strcpy( pData->m_car_secret, car.car_secret);// 设备密钥
	pData->m_reset_wifi2=car.reset_wifi2;// 重置wifi2标志位
	//check local property state
	//_refresh_local_property. if property changed, set sg_DataTemplate[i].state = eCHANGED;
		// 给属性更新是否更改标志
    for (i = 0, j = 0; i < TOTAL_PROPERTY_COUNT; i++) {       
        //if(eCHANGED == sg_DataTemplate[i].state) {
            pReportDataList[j++] = &(sg_DataTemplate[i].data_property);
						//sg_DataTemplate[i].state = eNOCHANGE;
						sg_DataTemplate[i].state = eCHANGED;
        //}
				//Log_d("【user data is update】%s:%lf",pReportDataList[i]->key,pReportDataList[i]->data);
    }
	*pCount = j;
	return (*pCount > 0)?QCLOUD_RET_SUCCESS:QCLOUD_ERR_FAILURE;
}

// 连接服务器，网络准备
static eAtResault net_prepare(void)
{
	eAtResault Ret;
	DeviceInfo sDevInfo;
	
	// 获取客户端实例
	at_client_t pclient = at_client_get();	
	
	// 获取设备信息
	memset((char *)&sDevInfo, '\0', sizeof(DeviceInfo));
	Ret = (eAtResault)HAL_GetDevInfo(&sDevInfo);
	if(QCLOUD_RET_SUCCESS != Ret){
		Log_e("Get device info err");
		return QCLOUD_ERR_FAILURE;
	}
	
	
	// 无线模块初始化
	if(QCLOUD_RET_SUCCESS != module_init(eMODULE_ESP8266)) 
	{
		Log_e("module init failed");
		goto exit;
	}else{
		Log_d("module init success");	
	}

	
	// 等待解析数据线程初始化
	while(AT_STATUS_INITIALIZED != pclient->status)
	{	
		HAL_SleepMs(1000);
	}
	
	
	// 等待连接无线模块
	Log_d("Start shakehands with module...");
	Ret = module_handshake(CMD_TIMEOUT_MS);
	if(QCLOUD_RET_SUCCESS != Ret)
	{
		Log_e("module connect fail,Ret:%d", Ret);
		goto exit;
	}else{
		Log_d("module connect success");
	}
	
	// 初始化iot设备信息，联网获取
	Ret = iot_device_info_init(sDevInfo.product_id, sDevInfo.device_name, sDevInfo.devSerc);
	if(QCLOUD_RET_SUCCESS != Ret)
	{
		Log_e("dev info init fail,Ret:%d", Ret);
		goto exit;
	}
	
	
	// 设置模块信息
	Ret = module_info_set(iot_device_info_get(), ePSK_TLS);
	if(QCLOUD_RET_SUCCESS != Ret)
	{
		Log_e("module info set fail,Ret:%d", Ret);
	}

exit:

	return Ret;
}

// 事件上报，检测
static void eventPostCheck(void *client)
{
#ifdef EVENT_POST_ENABLED	
	int rc;
	int i;
	uint32_t eflag;
	sEvent *pEventList[EVENT_COUNTS];
	uint8_t event_count;
		
	// 事件上报
	IOT_Event_setFlag(client, FLAG_EVENT0|FLAG_EVENT1|FLAG_EVENT2|FLAG_EVENT3);
	eflag = IOT_Event_getFlag(client);
	if((EVENT_COUNTS > 0 )&& (eflag > 0))
	{	
		event_count = 0;
		for(i = 0; i < EVENT_COUNTS; i++)
		{
		
			if((eflag&(1<<i))&ALL_EVENTS_MASK)
			{
				 pEventList[event_count++] = &(g_events[i]);				 
				 IOT_Event_clearFlag(client, 1<<i);
#ifdef	EVENT_TIMESTAMP_USED				 
				 update_events_timestamp(&g_events[i], 1);
#endif
			}			
		}	

		rc = IOT_Post_Event(client, sg_data_report_buffer, sg_data_report_buffersize, \
											event_count, pEventList, event_post_cb);
		if(rc < 0)
		{
			Log_e("events post failed: %d", rc);
		}
	}
#endif

}

// 获取设备系统信息，可自行添加信息字段，云端将保存设备信息字段
static int _get_sys_info(void *handle, char *pJsonDoc, size_t sizeOfBuffer)
{
	/*平台处理字段，必选字段至少有一个*/
    DeviceProperty plat_info[] = {
     	{.key = "module_hardinfo", .type = TYPE_TEMPLATE_STRING, .data = "ESP8266"},
     	{.key = "module_softinfo", .type = TYPE_TEMPLATE_STRING, .data = "V1.0"},
     	{.key = "fw_ver", 		   .type = TYPE_TEMPLATE_STRING, .data = QCLOUD_IOT_AT_SDK_VERSION},
//     	{.key = "imei", 		   .type = TYPE_TEMPLATE_STRING, .data = "11-22-33-44"},
//     	{.key = "lat", 			   .type = TYPE_TEMPLATE_STRING, .data = "22.546015"},
//     	{.key = "lon", 			   .type = TYPE_TEMPLATE_STRING, .data = "113.941125"},
        {NULL, NULL, JINT32}  //结束
	};
		
	/*自定义附加信息*/
	DeviceProperty self_info[] = {
        {.key = "test2", .type = TYPE_TEMPLATE_STRING, .data = "debug"},
        {NULL, NULL, JINT32}  //结束
	};

	return IOT_Template_JSON_ConstructSysInfo(handle, pJsonDoc, sizeOfBuffer, plat_info, self_info); 	
}

// 主任务，数据模板任务
void data_template_demo_task(void *arg)
{
	eAtResault Ret;
	int rc;
	uint8_t retry_count=0;

	void *client = NULL;
	// 获取客户端实例
	at_client_t pclient = at_client_get();	
	
	int ReportCont;
	DeviceProperty *pReportDataList[TOTAL_PROPERTY_COUNT];

/**
 * 进入主逻辑
 */
	Log_d("【data_template_demo_task init start】");
	do  
	{
		// 联网之前的准备
		Ret = net_prepare();
		if(QCLOUD_RET_SUCCESS != Ret)
		{
			Log_e("【net prepare fail,Ret:%d】", Ret);
		}
		Log_d("【init net prepare success】");
				// 更新看门狗
		HAL_IWDG_Refresh(&hiwdg);
		
RESET:	
		Log_d("【----entry net----】");
		

		// 无线模块初始化，包括连接mqtt服务器
		Ret = module_register_network(eMODULE_ESP8266);
		if(QCLOUD_RET_SUCCESS != Ret)
		{			
			Log_e("【network connect fail,Ret:%d】", Ret);
			goto RESET;
		}
		Log_d("【init network connect success】");
		// 联网成功
		
		// 初始化mqtt模板
		Ret = (eAtResault)IOT_Template_Construct(&client);
		if(QCLOUD_RET_SUCCESS != Ret)
		{
			Log_e("【data template construct fail,Ret:%d】", Ret);
			goto RESET;
		}
		Log_d("【init data template construct success】");

		
		// 初始化数据模板，内部实现
		_init_data_template();

				
		// 注册数据模板属性
		rc = _register_data_template_property(client);
		if (rc != QCLOUD_RET_SUCCESS) 
		{
			Log_e("【Register data template propertys Failed: %d】", rc);
			goto RESET;
		}
		Log_d("【init Register template propertys success】");

		
//		// 使能行为
//#ifdef ACTION_ENABLED
//		// 注册行为的数据模板
//		rc = _register_data_template_action(client);
//		if (rc != QCLOUD_RET_SUCCESS) {
//			Log_e("【Register data template actions Failed: %d】", rc);
//			goto RESET;
//		}Log_d("【init Register data template actions success】");
//#endif
			
		
//		// 上报设备信息,平台根据这个信息提供产品层面的数据分析,譬如位置服务等
//		rc = _get_sys_info(client, sg_data_report_buffer, sg_data_report_buffersize);
//		if(QCLOUD_RET_SUCCESS == rc)
//		{
//			rc = IOT_Template_Report_SysInfo_Sync(client, sg_data_report_buffer, sg_data_report_buffersize, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);	
//			if (rc != QCLOUD_RET_SUCCESS) 
//			{
//				Log_e("【Report system info fail, err: %d】", rc);
//				goto RESET;
//			}
//		}else{
//			Log_e("【Get system info fail, err: %d】", rc);
//			goto RESET;
//		}
//		Log_d("【init Report system info success】");

		
//		// 获取离线期间数据，状态
//		rc = IOT_Template_GetStatus_sync(client, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
//		if (rc != QCLOUD_RET_SUCCESS) 
//		{
//			Log_e("【Get data status fail, err: %d】", rc);
//			goto RESET;
//		}
//		Log_d("【init Get data status success】");


		
		
		Log_d("\r\n-----------【logic handle start】-----------");
		
		// 模式控制
		rc = control_mode(client,pReportDataList,ReportCont);
		if(rc!=QCLOUD_RET_SUCCESS)
		{
			Log_e("【logic handle fail, err: %d】", rc);// 上下行出错，重新初始化
			HAL_Delay(1000);
			retry_count++;
			if(retry_count>3) NVIC_SystemReset();// 系统复位
			goto RESET;
		}
		
//		while(1)
//		{
//			// 检测上行事件
//			eventPostCheck(client);
//			
//		}
		
		
	// 结束主逻辑循环
	}while (0);
	
	
	// 意外结束主任务，报错
	hal_thread_destroy(NULL);
	Log_e("Task teminated,Something goes wrong!!!");
}


// 处理小车状态
void handleMove(void)
{
	// 指令映射表
	// set_speed_whole:w-0
	// set_speed_angle:a-0
	// set_speed_dir:d-0
	// set_dir_speed_whole:w-1
	// wifi2_reset: r-1
	// system_reset: r-2 
	// receive_app: r-3 
	// entry_app: a-1 

	char *p;
	
	// 转换速度
	p = strstr((const char*)UART5_RX_BUF,"w-0");
	if(p) car.set_speed_whole=atoi(p+5);
	
	// 转换角度
	p = strstr((const char*)UART5_RX_BUF,"a-0");
	if(p) car.set_speed_angle=atoi(p+5);
	
	// 转换移动方向
	p = strstr((const char*)UART5_RX_BUF,"d-0");
	if(p) car.set_speed_dir=atoi(p+5);
	
	// 转换旋转速度
	p = strstr((const char*)UART5_RX_BUF,"w-1");
	if(p) car.set_dir_speed_whole=atoi(p+5);
	
	// 进入bootloader
	p = strstr((const char*)UART5_RX_BUF,"r-2");
	if(p){
		NVIC_SystemReset();
	}
	
	
	update_move();// 更新小车移动设置
	
	//Log_d("set_speed_whole:%d,set_speed_angle:%d,set_speed_dir:%d,set_dir_speed_whole:%d",car.set_speed_whole,car.set_speed_angle,car.set_speed_dir,car.set_dir_speed_whole);

	// 开启下一次接收
	UART5_RX_STA=0;
	HAL_UART_Receive_IT(&huart5,&UART5_RX_BUF[UART5_RX_STA],1);
}

// 选择控制模式
int control_mode(void *client,DeviceProperty *pReportDataList[],int ReportCont)
{
	int rc;
	uint8_t fail_count=0;
	uint16_t i=0;
	uint16_t n=80;
	uint16_t x=80;
	uint16_t c=160;
	uint8_t * car_data="{\"car_secret\":\"VlzbGfFuvRV8o67sTlRLMw==\"}";
	
	// 更新看门狗
	HAL_IWDG_Refresh(&hiwdg);
	flicker_led(2,1,1,0,2000);//双灯长亮3秒

	Log_d("\r\n#####【start handle cloud logic】#####");
	while(1)
	{
		// 25ms 周期
		if(++i>=50000) i=0,n++,c++,x++;
		
		//维持连接
		if(c>=60){
			c=0;
			HAL_DCMI_Suspend(&hdcmi);// 暂停捕获图像
			while(HAL_UART_Transmit_IT(&huart5,car_data,strlen((const char *)car_data)));
			HAL_DCMI_Resume(&hdcmi);// 恢复捕获
		}

		if(is_start)
		{
			// 处理图像阶段
			if(img_data_ok){
				HAL_DCMI_Suspend(&hdcmi);// 暂停捕获图像
				__HAL_DMA_DISABLE(&hdma_dcmi);//关闭DMA
				while(DMA2_Stream1->CR&0X01);	//等待DMA2_Stream1可配置   
				// 获取本次采集的一帧数据长度，必须小于buf
				img_data_len=(img_buf_size-__HAL_DMA_GET_COUNTER(&hdma_dcmi))/2+1;
				while(HAL_UART_Transmit_DMA(&huart5,(uint8_t *)img_buf,img_data_len*4));
				if(n>=40){// 2s周期，更新小车状态和逻辑处理
					
					n=0;
					rc = logic_handle(client,pReportDataList,ReportCont);
					if(rc!=QCLOUD_RET_SUCCESS&&++fail_count>3) return rc;
					else fail_count=0;
					
				}else if(x>=60){
					
					x=0;
					update_state();// 抽空更新舵机和摄像头、wifi2重置
					
				}
				while(HAL_UART_Transmit_DMA(&huart5,(uint8_t *)img_buf+img_data_len*4,img_data_len*4));
				// 串口5接收处理
				if((UART5_RX_STA&0x8000)!=0) handleMove();
				while(huart5.gState!=HAL_UART_STATE_READY);
				resume_and_start_next();
			}
			HAL_IWDG_Refresh(&hiwdg);
		}else{
			// 串口5接收处理
			if((UART5_RX_STA&0x8000)!=0) handleMove();
			if(n>=80){// 2s周期，更新小车状态和逻辑处理
			
				n=0;
				HAL_IWDG_Refresh(&hiwdg);
				rc = logic_handle(client,pReportDataList,ReportCont);
				if(rc!=QCLOUD_RET_SUCCESS&&++fail_count>3) return rc;
				else fail_count=0;
				update_state();// 抽空更新舵机和摄像头、wifi2重置
				
			}
		}
		
	}
}

// 上下行逻辑处理
int logic_handle(void *client,DeviceProperty *pReportDataList[],int ReportCont)
{
	int rc;
	
	IOT_Template_Yield(client, 1);
	// 服务端下行消息到达，可以开始处理服务器指令
	if (sg_control_msg_arrived) {	
		
		// 下行业务逻辑处理
		deal_down_stream_user_logic(client, &sg_ProductData);	
		
		
		//业务逻辑处理完后需要通知服务端control msg 已收到，请服务端删除control msg，否则服务端会保留control msg(通过Get status命令可以得到未删除的Control数据)
		sReplyPara replyPara;
		memset((char *)&replyPara, 0, sizeof(sReplyPara));
		replyPara.code = eDEAL_SUCCESS;
		replyPara.timeout_ms = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;						
		replyPara.status_msg[0] = '\0';			//可以通过 replyPara.status_msg 添加附加消息，一般在失败情况下才添加
		
		// 回复下行control消息
		rc = IOT_Template_ControlReply(client, sg_data_report_buffer, sg_data_report_buffersize, &replyPara);
		if (rc == QCLOUD_RET_SUCCESS) {
			//Log_d("Contol msg reply success");
			sg_control_msg_arrived = false;   
		}else{
			Log_e("【Contol msg reply failed, err: %d】", rc);
			return rc;
		}				
	}	

//HAL_Delay(1000);

	// 设备上行给服务器内容，主动上报
	if(QCLOUD_RET_SUCCESS == deal_up_stream_user_logic(client, pReportDataList, &ReportCont,&sg_ProductData)){
		
		// 给上报数据添加回复信息，添加reported字段，不覆盖更新
		rc = IOT_Template_JSON_ConstructReportArray(client, sg_data_report_buffer, sg_data_report_buffersize, ReportCont, pReportDataList);
		if (rc == QCLOUD_RET_SUCCESS) {
			// 数据模板异步方式上报数据
			rc = IOT_Template_Report(client, sg_data_report_buffer, sg_data_report_buffersize, 
										OnReportReplyCallback, NULL, QCLOUD_IOT_MQTT_COMMAND_TIMEOUT);
			if (rc == QCLOUD_RET_SUCCESS) {
				//Log_d("data template reporte success");
			}else{
				Log_e("【data template reporte failed, err: %d】", rc);
				return rc;
			}
		}else{
			Log_e("【construct reporte data failed, err: %d】", rc);
			return rc;
		}
	}
	return QCLOUD_RET_SUCCESS;
}


// 创建主任务线程
void data_template_sample(void)
{
	osThreadId demo_threadId;
	
#ifdef OS_USED
	hal_thread_create(&demo_threadId, 512, osPriorityNormal, data_template_demo_task, NULL);
	hal_thread_destroy(NULL);
#else
	#error os should be used just now
#endif
}
