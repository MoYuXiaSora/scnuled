#include "delay.h" 			 
#include "usart.h" 			 
#include "usart3.h" 			 
#include "hc05.h"  
#include "string.h"	 
#include "math.h"
#include "frame.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-HC05蓝牙模块驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/3/29
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20140329
//修改LED的连接，原来接PC5，改为PA4，以兼容MiniSTM32开发板V3.0									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化ATK-HC05模块
//返回值:0,成功;1,失败.

//blooth send bytes 

static u8 BLE_RX_STA[USART3_MAX_RECV_LEN];
int BLE_SERVICE_STATUS ;

 u8 HC05_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	BLE_SERVICE_STATUS = 0 ;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//使能PORTA
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化A15
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA4

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
	usart3_init(115200);	//初始化串口3为:115200,波特率
  BlueToothInitTest();
	delay_ms(10);
	BlueToothNameCheck();
	delay_ms(10);
	BlueToothcfgeCheck();
	BLE_SERVICE_STATUS = 1 ;
  	
	//wifi
	
}

u8 BlueToothInitTest(void){
	u8 InitRes ;
	InitRes = HC05_Set_Cmd("AT+BLEINIT=2\r\n","OK");
	return InitRes ;
}


u8 BlueToothNameCheck(void){
	u8 InitRes ;
	InitRes = HC05_Set_Cmd("AT+BLENAME=\"L_led\"\r\n","OK");
	return InitRes ;
}


u8 BlueToothcfgeCheck(void){
	u8 InitRes ;
	HC05_Set_Cmd("AT+CWMODE=0\r\n","OK");
	delay_ms(10);
	HC05_Set_Cmd("AT+SYSMSG=7\r\n","OK");
  delay_ms(10);	
	HC05_Set_Cmd("AT+BLEGATTSSRVCRE\r\n","OK");
	delay_ms(10);
	HC05_Set_Cmd("AT+BLEGATTSSRVSTART\r\n","OK");
	delay_ms(10);
	HC05_Set_Cmd("AT+BLEADVSTART\r\n","OK");
  delay_ms(10);	
	HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n","OK");
	delay_ms(10);
  BLE_SERVICE_STATUS =1 ;
	memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);	
	return InitRes ;
}

u8 BlueToothMsgManage(u8* atstr){
	char *disconnloc ;
	char *connloc ;
	char *writeloc ;
	disconnloc = strstr(USART3_RX_BUF,"BLEDISCONN");
	connloc = strstr(USART3_RX_BUF,"BLECFGMTU:");
	writeloc = strstr(USART3_RX_BUF,"WRITE");
	u1_printf("status+ %x ",BLE_SERVICE_STATUS);
	u1_printf("come into msgmanage");
	u1_printf(USART3_RX_BUF);	
	if(disconnloc!=0){
		HC05_Set_Cmd("+++","");
		BLE_SERVICE_STATUS = 1 ;
		delay_ms(1000);
    HC05_Set_Cmd("AT+BLEADVSTART\r\n","OK");
		HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n","OK");
    USART3_RX_STA=0 ;			
		return 0 ;
	}
	else if (connloc!=0){
		BLE_SERVICE_STATUS = 2;
		memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
	  USART3_RX_STA=0 ;	
		return 0 ;
	}
		else if(writeloc!=0){
		BLE_SERVICE_STATUS = 0 ;
		HC05_Set_Cmd("AT+BLESPPCFG=1,1,6,1,5\r\n","OK");
		delay_ms(10);
		HC05_Set_Cmd("AT+BLESPP\r\n","OK");
		memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
		BLE_SERVICE_STATUS = 3 ;
		USART3_RX_STA=0;
	}
	else{
		memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
	  USART3_RX_STA=0 ;	
		return 0 ;
	}
}

u8 BlueToothSPPManage(u8* atstr){
	char *disconnloc ;
	char *writeloc ;
	u1_printf("status+ %x ",BLE_SERVICE_STATUS);
	u1_printf("come into sppmanage");
	u1_printf(USART3_RX_BUF);
	disconnloc = strstr(USART3_RX_BUF,"BLEDISCONN");
	writeloc = strstr(USART3_RX_BUF,"WRITE");
	if(disconnloc!=0){
		HC05_Set_Cmd("+++","");
		BLE_SERVICE_STATUS = 0 ;
		delay_ms(1000);
    HC05_Set_Cmd("AT+BLEADVSTART\r\n","OK");
		HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n","OK");
		BLE_SERVICE_STATUS = 1 ;
		memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
    USART3_RX_STA=0;		
		return 0 ;
	}
	else if(writeloc!=0){
		BLE_SERVICE_STATUS = 0 ;
		HC05_Set_Cmd("AT+BLESPPCFG=1,1,6,1,5\r\n","OK");
		delay_ms(10);
		HC05_Set_Cmd("AT+BLESPP\r\n","OK");
		memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
		BLE_SERVICE_STATUS = 3 ;
		USART3_RX_STA=0;
	}
	
};



//代码状态修改 这里看看要不要先置为0

u8 BlueToothCmdManage(u8* atstr){
	u8 InitRes ;
	char *disconnloc ;
	char * writeloc;
	u1_printf("status+ %x ",BLE_SERVICE_STATUS);
	u1_printf("come into CMDmanage");
	disconnloc = strstr(USART3_RX_BUF,"BLEDISCONN");
	writeloc   = strstr(USART3_RX_BUF,"WRITE");
	if(disconnloc!=0){
		HC05_Set_Cmd("+++","");
		BLE_SERVICE_STATUS = 0 ;
		delay_ms(1000);
    HC05_Set_Cmd("AT+BLEADVSTART\r\n","OK");
		HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n","OK");
		BLE_SERVICE_STATUS = 1 ;
		memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
    USART3_RX_STA=0;		
		return 0 ;
	}
	else if(writeloc!=0){
		BLE_SERVICE_STATUS = 0 ;
		HC05_Set_Cmd("AT+BLESPPCFG=1,1,6,1,5\r\n","OK");
		delay_ms(10);
		HC05_Set_Cmd("AT+BLESPP\r\n","OK");
		memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
		BLE_SERVICE_STATUS = 3 ;
		USART3_RX_STA=0;
	}
}




//ATK-HC05设置命令
//此函数用于设置ATK-HC05,适用于仅返回OK应答的AT指令
//atstr:AT指令串.比如:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"等字符串
//返回值:0,设置成功;其他,设置失败.							  
u8 HC05_Set_Cmd(u8* atstr , u8* check ){
	char *loc ;
 	u8 retry = 0X05;
	u8 temp,t;
	memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
	u1_printf(atstr);
	while(retry--)
	{
		delay_ms(10);
		u3_printf(atstr,0);	//发送AT字符串
		for(t=0;t<20;t++) 			//最长等待100ms,来接收HC05模块的回应
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)break;
		}		
		if(USART3_RX_STA&0X8000)	//接收到一次数据了
		{
			temp=USART3_RX_STA&0X7FFF;	//得到数据长度
			USART3_RX_STA=0;
      loc = strstr(USART3_RX_BUF,check);	
			if(loc!=0)
			{
				temp=0;//接收到OK响应
				break;
			}
		}		
	}
	if(retry==0){return 0;}//设置失败.
	u1_printf("OK!");
	u1_printf(USART3_RX_BUF);
	return 1;} 











