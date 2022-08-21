#ifndef __ESP8266_H
#define __ESP8266_H
#include "main.h" 	 
#include "string.h"	 

#define UART5_MAX_RECV_LEN		100					//最大接收缓存字节数
#define UART5_MAX_SEND_LEN		100					//最大发送缓存字节数

extern uint8_t UART5_RX_BUF[UART5_MAX_RECV_LEN]; //接收缓冲,最大USART3_MAX_RECV_LEN个字节.
extern uint8_t init_wifi2_finish;// wifi2是否初始化完成
extern uint16_t UART5_RX_STA;


void u5_printf(char* fmt,...);
void atk_8266_at_response(uint8_t mode);
uint8_t* atk_8266_check_cmd(uint8_t *str);
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);
uint8_t atk_8266_online_test(void);

uint8_t atk_8266_quit_trans(void);
uint8_t atk_8266_set_trans(void);
uint8_t atk_8266_set_uart(uint8_t *p);
uint8_t atk_8266_close_connect(uint8_t *ip);
uint8_t atk_8266_set_mode(uint8_t mode);
void atk_8266_init(void);
uint8_t sta_connect_wifi(void);
uint8_t set_sta_tcp(void);
uint8_t atk_8266_sta_start(uint8_t netpro);
uint8_t set_uart_baudRate(uint32_t baudRate);
uint8_t set_esp8266_baudRate(uint8_t * baudRate);
uint8_t atc_8266_connect(void);
#endif


