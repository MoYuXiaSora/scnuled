#include "delay.h" 			 
#include "usart.h" 			 
#include "usart3.h" 			 
#include "hc05.h"  
#include "string.h"	 
#include "math.h"
#include "frame.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-HC05����ģ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/3/29
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20140329
//�޸�LED�����ӣ�ԭ����PC5����ΪPA4���Լ���MiniSTM32������V3.0									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��ATK-HC05ģ��
//����ֵ:0,�ɹ�;1,ʧ��.

//blooth send bytes 

static u8 BLE_RX_STA[USART3_MAX_RECV_LEN];
int BLE_SERVICE_STATUS ;

 u8 HC05_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	BLE_SERVICE_STATUS = 0 ;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//ʹ��PORTA
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��A15
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA4

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
	usart3_init(115200);	//��ʼ������3Ϊ:115200,������
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



//����״̬�޸� ���￴��Ҫ��Ҫ����Ϊ0

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




//ATK-HC05��������
//�˺�����������ATK-HC05,�����ڽ�����OKӦ���ATָ��
//atstr:ATָ�.����:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"���ַ���
//����ֵ:0,���óɹ�;����,����ʧ��.							  
u8 HC05_Set_Cmd(u8* atstr , u8* check ){
	char *loc ;
 	u8 retry = 0X05;
	u8 temp,t;
	memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
	u1_printf(atstr);
	while(retry--)
	{
		delay_ms(10);
		u3_printf(atstr,0);	//����AT�ַ���
		for(t=0;t<20;t++) 			//��ȴ�100ms,������HC05ģ��Ļ�Ӧ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)break;
		}		
		if(USART3_RX_STA&0X8000)	//���յ�һ��������
		{
			temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
			USART3_RX_STA=0;
      loc = strstr(USART3_RX_BUF,check);	
			if(loc!=0)
			{
				temp=0;//���յ�OK��Ӧ
				break;
			}
		}		
	}
	if(retry==0){return 0;}//����ʧ��.
	u1_printf("OK!");
	u1_printf(USART3_RX_BUF);
	return 1;} 











