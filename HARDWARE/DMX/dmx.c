#include "delay.h" 			 
#include "usart.h" 			 
#include "usart2.h" 			 
#include "hc05.h" 
#include "led.h" 
#include "string.h"	 
#include "math.h"
#include "dmx.h"
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

//��ʼ��DMXģ��
//����ֵ:0,�ɹ�;1,ʧ��.


 u8 DMX_Init(void)
{
	u8 retry=10,t;	  		 
	u8 temp=1;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//ʹ��PORTA PA2 PA3
 
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
}





