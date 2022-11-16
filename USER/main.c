#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "hc05.h"
#include "usart3.h"			 	 
#include "string.h"	   
#include "usmart.h"	
#include "usart2.h"	
#include "dmx.h"
#include "rs485.h"


//write异常
int main(void)
{
	u8 t = 0;
	u8 key;
	u8 sendmask = 0;
	u8 sendcnt = 0;
	u8 sendbuf[20];
	u8 reclen = 0;
	u8 c = 0;
	u8 role = 0;
	u8 rs485buf[35];
	u8 i = 0;
	u8 cnt = 0;
	u8 co;

	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	usmart_dev.init(72); 	//初始化USMART		 
	delay_ms(500);			//等待蓝牙模块上电稳定
	HC05_Init(); //初始化蓝牙模块
	RS485_Init(250000);
	delay_ms(100);

	while (1) {}
}

