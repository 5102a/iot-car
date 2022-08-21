
#include "esp8266.h"
#include "usart.h"
#include "tim.h"
#include "stdarg.h"
#include "iwdg.h"
#include "main.h"


//用户配置区

//连接端口号:8888,可自行修改为其他端口.
uint8_t* remotePort="58266";		
uint8_t* remoteIp="118.25.95.166";	

//uint8_t* remotePort="8888";		 
//uint8_t* remoteIp="192.168.137.1";	

// 本地端口号tcp本地端口9999
uint8_t* localTcpPort="9998";	
uint8_t* keepAliveTime="60";


//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
//uint8_t* wifista_ssid="ChinaNet-9rrY";			//路由器SSID号
//uint8_t* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
//uint8_t* wifista_password="jc2ttcwb"; 	//连接密码

uint8_t* wifista_ssid="mycar";			//路由器SSID号
uint8_t* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
uint8_t* wifista_password="44466666"; 	//连接密码


//串口发送缓存区 	
__align(4) uint8_t UART5_TX_BUF[UART5_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节
uint8_t UART5_RX_BUF[UART5_MAX_RECV_LEN] __attribute__ ((at(0X20001000))); //接收缓冲,最大USART3_MAX_RECV_LEN个字节.

//接收到的数据状态
//[31]:0,没有接收到数据;1,接收到了一批数据.
//[30:0]:接收到的数据长度
uint32_t UART5_RX_STA=0;   	 

uint8_t init_wifi2_finish = 0;

void u5_printf(char* fmt,...)  
{  
	uint16_t i;
	va_list ap;// 定义可变参数列表
	va_start(ap,fmt);// 首个可变参数地址赋值给ap
	vsprintf((char*)UART5_TX_BUF,fmt,ap);// 把ap以fmt的格式存入发送缓冲
	va_end(ap);// 结束可变参数
	// 获取这次发送的数据长度并发送
	i=strlen((const char*)UART5_TX_BUF);//此次发送数据的长度
	// 发送数据
	HAL_UART_Transmit_IT(&huart5,UART5_TX_BUF,i);
}

// 返回响应数据给pc
void atk_8266_at_response(uint8_t mode)
{
	if((UART5_RX_STA&0x80000000)==0x80000000)		//接收到一次数据了
	{ 
		printf("\r\n-----response-----\r\n%s\r\n-----end----\r\n",UART5_RX_BUF);	//发送到串口pc
		if(mode) UART5_RX_STA=0;
	} 
}

//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
uint8_t* atk_8266_check_cmd(uint8_t *str)
{
	char *strx=0;
	if((UART5_RX_STA&0x80000000)==0x80000000)		//接收到一次完整数据了
	{ 
		// 查找并返回接收数据中第一次出现str的位置索引，字符串指针
		strx=strstr((const char*)UART5_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}

//向ATK-ESP8266发送命令
//0,发送成功(得到了期待的应答结果) 1,发送失败
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	printf("\r\nSEND CMD:%s\r\n",cmd);
	uint8_t res=0; 
	UART5_RX_STA=0;
	if(cmd[0]=='+'&&cmd[1]=='+'&&cmd[2]=='+')
	{
		// 无需接收
		u5_printf("%s",cmd);
		return 0;
	}else{
		HAL_UART_Receive_IT(&huart5,UART5_RX_BUF,1);// 开启接收
		u5_printf("%s\r\n",cmd);
	}
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			HAL_Delay(10);
			
			if((UART5_RX_STA&0x80000000)==0x80000000)//接收到期待的应答结果
			{
				// 得到期待应答
				if(atk_8266_check_cmd(ack)) break;
			} 
		}
		if(waittime==0) res=1; // 未等到预期结果
	}
	atk_8266_at_response(0);// 返回响应给pc
	return res;
} 


//ATK-ESP8266退出透传模式
//0,退出成功;1,退出失败
uint8_t atk_8266_quit_trans(void)
{
	atk_8266_send_cmd("+++","",0);
	HAL_Delay(1100);					//等待1100ms
	atk_8266_send_cmd("AT","OK",100);
	return atk_8266_send_cmd("AT","OK",100);
}

//ATK-ESP8266设置透传模式
//0成功;1失败
uint8_t atk_8266_set_trans(void)
{
	uint8_t restart=0;
	// 配置透传模式
	if(atk_8266_send_cmd("AT+CIPMODE=1","OK",200)==0) return 0;
	
	while(atk_8266_send_cmd("AT+CIPMODE?","CIPMODE:1",500))
	{
		if(atk_8266_send_cmd("AT+CIPMODE=1","OK",200)==0) break;
		if(++restart>2){return 1;}
		HAL_Delay(200);
	}
	return 0;
}

//ATK-ESP8266设置波特率
//0成功;1失败
uint8_t atk_8266_set_uart(uint8_t *p)
{
	uint8_t restart=0,i=0;
	uint8_t cmd[] = "AT+UART_CUR=115200,8,1,0,0";
	for(i=0;i<=5;i++)
	{
		cmd[i+12]=p[i];
	}
	while(atk_8266_send_cmd(cmd,"OK",100))
	{
		if(++restart>2){return 1;}
		HAL_Delay(200);
	}
	printf("###【esp8266波特率设置成功】###\r\n");
	return 0;
}


// 模块在线测试，进入AT模式	 0在线，1离线
uint8_t atk_8266_online_test(void)
{
	uint8_t restart=0;
	if(atk_8266_send_cmd("AT","OK",100)==0) return 0;
	// 发送测试指令给模块，检查WIFI模块是否在线
	while(atk_8266_quit_trans())// 如果发送失败，则一直发送，直到成功
	{
		if(++restart>1){return 1;}// AT指令无效，退出失败或者离线
		HAL_Delay(200);
	} 
	return 0;
}


// 设置8266模式1 sta，2 ap,3 ap+sta; 0成功，1失败
uint8_t atk_8266_set_mode(uint8_t mode)
{
	uint8_t restart=0;
	uint8_t ack[] = "CWMODE_DEF:1";
	uint8_t cmd[] = "AT+CWMODE_DEF=1";
	ack[11] = mode + '0';
	cmd[14] = mode + '0';

	if(atk_8266_send_cmd(cmd,"OK",100)==0) return 0;
	// 检测，配置为模式1
	while(atk_8266_send_cmd("AT+CWMODE_DEF?",ack,500))
	{
		if(atk_8266_send_cmd(cmd,"OK",100)==0) break;
		if(++restart>2){restart=0;return 1;}
		HAL_Delay(200);
	}
	return 0;
}


// sta模式下连接到wifi
uint8_t sta_connect_wifi(void)
{
	uint8_t restart=0;
	uint8_t p[30];
	
	// 更新配置，默认配置掉电后仍保留
	sprintf((char*)p,"AT+CWJAP_DEF=\"%s\",\"%s\"",wifista_ssid,wifista_password);
	
	// 配置sta 连接到wifi
	while(atk_8266_send_cmd("AT+CWJAP_DEF?","CWJAP_DEF:\"",800)) // 存在默认连接
	{
		if(atk_8266_check_cmd(wifista_ssid) && atk_8266_check_cmd(wifista_password)) break;
		// 断开热点
		atk_8266_send_cmd("AT+CWQAP","OK",400);
		HAL_IWDG_Refresh(&hiwdg);
		if(atk_8266_send_cmd(p,"CONNECTED",2000)==0) break;					//设置AP模式参数
		if(++restart>2) return 1;
		HAL_Delay(200);
	}
	return 0;
}

//sta模式配置sta为tcp，0 成功，1失败
uint8_t set_sta_tcp(void)
{
	uint8_t restart=0;
	uint8_t p[60];
	atk_8266_send_cmd("AT+CIPCLOSE","OK",100);
	// 配置sta tcp客户端工作模式
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s,300",remoteIp,remotePort);
	
	// 先判断是否存在连接
	while(atk_8266_send_cmd("AT+CIPSTATUS",remoteIp,100)||!atk_8266_check_cmd(remotePort))
	{
		if(atk_8266_check_cmd(remoteIp))
		{
			atk_8266_send_cmd("AT+CIPCLOSE","OK",100);
		}
		if(atk_8266_send_cmd(p,"OK",500)==0) break;
		if(++restart>2) return 1;
		HAL_Delay(200);
	}
	printf("###【STA TCP已注册】###\r\n");
	return 0;
}

// 启动sta并连接进入透传
uint8_t atk_8266_sta_start(uint8_t netpro)
{
	// 检查或者配置模式3
	if(atk_8266_set_mode(netpro))
	{
		printf("###【配置模式1失败】###\r\n");return 1;
	}
	
	// 设置单连接
	if(atk_8266_send_cmd("AT+CIPMUX=0","OK",100))
	{
		printf("###【sta模式设置单连接失败】###\r\n");
	}
	

_RESET:
	// 退出smartCOnfig配网模式
	if(atk_8266_send_cmd("AT+CWSTOPSMART","OK",500))
	{
		printf("###【AT+CWSTOPSMART失败】###\r\n");
	}
	
	if(ENTRY_NETWORK_METHOD){
		flicker_led(2,1,1,1,1);
		HAL_IWDG_Refresh(&hiwdg);
		// 进入smartConfig配网模式
		if(atk_8266_send_cmd("AT+CWSTARTSMART=3","connected",2000))
		{
			printf("###【SmartConfig配网失败】###\r\n");
		}
		flicker_led(2,0,1,0,1);
		
		// 退出smartCOnfig配网模式
		if(atk_8266_send_cmd("AT+CWSTOPSMART","OK",500))
		{
			printf("###【AT+CWSTOPSMART失败】###\r\n");
		}
		HAL_IWDG_Refresh(&hiwdg);
		// 判断是否已连接
		if(atk_8266_send_cmd("AT+CWJAP?","CWJAP",1000)){
			goto _RESET;
		}
	}else{
		// 默认配网
		if(sta_connect_wifi())
		{
			printf("###【sta模式连接wifi失败】###\r\n");return 1;
		}
	}
	HAL_IWDG_Refresh(&hiwdg);
	// 连接wifi后延迟，以便初始化连接
	HAL_Delay(1000);
	
	// 查询ip
	atk_8266_send_cmd("AT+CIFSR","OK",100);

	
	// 配置sta工作模式 tcp
	if(set_sta_tcp()) 
	{
		printf("###【sta tcp注册失败】###\r\n");return 1;
	}
	
	// 只在单连接tcp客户端和upd下可设置透传模式，未进入
	if(atk_8266_set_trans()) 
	{
		printf("###【设置透传失败】###\r\n");
		return 1;
	}
	return 0;		
} 

// 设置串口波特率
uint8_t set_uart_baudRate(uint32_t baudRate)
{
		// 修改串口5波特率
	__HAL_UART_DISABLE(&huart5);
	huart5.Init.BaudRate = baudRate;
	if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    printf("###【change uart5 BaudRate fail】###\r\n");
		return 1;
  }
	printf("###【change uart5 BaudRate:%d success】###",baudRate);
	return 0;
}

// 设置esp8266波特率
uint8_t set_esp8266_baudRate(uint8_t * baudRate)
{
	// 设置波特率 921600 460800 115200 
	if(atk_8266_set_uart(baudRate))
	{
		printf("###【change esp8266 BaudRate fail】###\r\n");
		return 1;
	}
	printf("###【change esp8266 BaudRate:%s success】###",baudRate);
	return 0;
}

// 尝试和wifi2简建立串口通信
uint8_t atc_8266_connect()
{
	// AT 指令无效
	if(atk_8266_send_cmd("AT","OK",100))
	{
		// 退出透传，进入AT模式无效
		if(atk_8266_quit_trans())
		{
			// 设置串口波特率
			set_uart_baudRate(115200);
			HAL_Delay(100);
			if(atk_8266_send_cmd("AT","OK",100))
			{
				if(atk_8266_quit_trans()) 
				{
					return 1;
				}
			}
		}
	}
	// 设置esp8266波特率 1382400 921600 691200 460800 115200 57600 
	set_esp8266_baudRate("115200");
	printf("###【wifi2通信成功】###\r\n");
	return 0;
}

//ATK-ESP8266模块测试主函数
void atk_8266_init(void)
{

	uint8_t restart=2;
	printf("###【bootloader初始化】###\r\n");
	printf("###【开始初始化wifi2】###\r\n");

	// 开始通信
	if(atc_8266_connect())
	{
		printf("###【wifi2通信失败,开始复位】###\r\n");
		// uart5 硬件复位
		HAL_GPIO_WritePin(WIFI2_RESET_GPIO_Port,WIFI2_RESET_Pin,GPIO_PIN_RESET);
		HAL_Delay(300);
		HAL_GPIO_WritePin(WIFI2_RESET_GPIO_Port,WIFI2_RESET_Pin,GPIO_PIN_SET);
		printf("###【复位完成】###\r\n");
	}
	
	// 设置串口波特率，配置esp8266低波特率稳定
	set_uart_baudRate(115200);
	HAL_IWDG_Refresh(&hiwdg);
RESET:
	
	while(--restart)
	{
		// 发送测试指令给模块，检查WIFI模块是否在线
		if(atk_8266_online_test())
		{
			printf("###【设备离线或无法连接，开始复位】###\r\n");
			// uart5 复位
			HAL_GPIO_WritePin(WIFI2_RESET_GPIO_Port,WIFI2_RESET_Pin,GPIO_PIN_RESET);
			HAL_Delay(300);
			HAL_GPIO_WritePin(WIFI2_RESET_GPIO_Port,WIFI2_RESET_Pin,GPIO_PIN_SET);
			printf("###【复位完成】###\r\n");
		}else{
			printf("###【设备在线】###\r\n");
			break;
		}
	}
	
	//关闭回显
	while(atk_8266_send_cmd("ATE0","OK",100))
	{
		if(++restart>2){break;}
		HAL_Delay(200);
	}
	HAL_IWDG_Refresh(&hiwdg);

	
	// 进入sta模式
	if(atk_8266_sta_start(1))
	{
		printf("###【init tcp esp8266 fail】###\r\n");
		goto RESET;
	}
	printf("【init tcp esp8266 success】\r\n");
	HAL_IWDG_Refresh(&hiwdg);
	
	// 设置esp8266波特率 1382400 921600 806400 748800 691200 460800 115200 57600 
	set_esp8266_baudRate("691200");
	
	// 设置串口波特率
	set_uart_baudRate(691200);
	
	if(atk_8266_send_cmd("AT+CIPSEND","OK",500))
	{
		printf("###【透传进入失败】###\r\n");
		goto RESET;
	}
	printf("###【透传已开启并进入】###\r\n");
	printf("###【wifi2初始化成功】###\r\n");
	printf("###【bootloader初始化成功】###\r\n");
	
	init_wifi2_finish=1;
	HAL_TIM_Base_Stop_IT(&htim14);// 终止串口5接收计时器
	UART5_RX_STA=0;
	HAL_Delay(500);
	HAL_UART_Receive_IT(&huart5,&UART5_RX_BUF[0],1);
	flicker_led(2,2,1,0,1000);
}

