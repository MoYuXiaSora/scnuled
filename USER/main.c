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


 
 /************************************************
 ��չʵ��1��ALIENTEK STM32F103������ 
 ATK-HC05��������ģ��ʵ��-�⺯���汾  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
	
	

//write�쳣
 int main(void)
	 {	 
	u8 t = 0;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 reclen=0;
  u8 c = 0 ;
  u8 role = 0 ;	
  u8 rs485buf[35];
  u8 i=0;
	u8 cnt=0	; 
	u8 co;

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
  usmart_dev.init(72); 	//��ʼ��USMART		 
	delay_ms(500);			//�ȴ�����ģ���ϵ��ȶ�
 // RS485_Init(250000);
	HC05_Init(); //��ʼ��ATK-HC05ģ�� 
	delay_ms(100);
	
	while(1){}
}
	
