#include "delay.h" 			 
#include "usart.h" 			 
#include "usart3.h" 			 
#include "hc05.h"  
#include "string.h"	 
#include "math.h"
#include "frame.h"


//初始化ATK-HC05模块
//返回值:0,成功;1,失败.

//BLEstatus：0 未初始化和数据处理 1：已初始化但未连接 2：已连接并开启SPP
typedef enum {
    BLE_IDLE = 0,
    BLE_READY,
    BLE_CONNECTED,
    BLE_SPP
} Ble_Service_Status;

Ble_Service_Status BLE_SERVICE_STATUS = BLE_IDLE;

/*************************************************
  Function:       // HC05_Init
  Description:    // 初始化串口和蓝牙模块
  Input:          // 无
  Return:         // 无
  Others:         // 其它说明
*************************************************/
u8 HC05_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	BLE_SERVICE_STATUS = 0;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PORTA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化A15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA4
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	usart3_init(115200);	//初始化串口3为:115200,波特率
	BlueToothInitTest();
	delay_ms(10);
	BlueToothNameCheck();
	delay_ms(10);
	BlueToothcfgeCheck();
	BLE_SERVICE_STATUS = 1;
}

/*************************************************
  Function:       // BlueToothInitTes
  Description:    // 设置蓝牙模块工作模式
  Input:          // 无
  Return:         // 设置结果
  Others:         // 结果（0：成功 ，1：失败）
*************************************************/
u8 BlueToothInitTest(void)
{
	u8 InitRes;
	InitRes = HC05_Set_Cmd("AT+BLEINIT=2\r\n", "OK", 0);
	return InitRes;
}

/*************************************************
  Function:       // BlueToothNameCheck
  Description:    // 设置蓝牙模块名称
  Input:          // 无
  Return:         // 设置结果
  Others:         // 结果（0：成功 ，1：失败）
*************************************************/
u8 BlueToothNameCheck(void)
{
	u8 InitRes;
	InitRes = HC05_Set_Cmd("AT+BLENAME=\"L_led\"\r\n", "OK", 0);
	return InitRes;
}

/*************************************************
  Function:       // BlueToothcfgeChec
  Description:    // 蓝牙模块初始化
  Input:          // 无
  Return:         // 设置结果
  Others:         // 结果（0：成功 ，1：失败）
*************************************************/
u8 BlueToothcfgeCheck(void)
{
	u8 InitRes;
	HC05_Set_Cmd("AT+CWMODE=0\r\n", "OK", 0);
	delay_ms(10);
	HC05_Set_Cmd("AT+SYSMSG=7\r\n", "OK", 0);
	delay_ms(10);
	HC05_Set_Cmd("AT+BLEGATTSSRVCRE\r\n", "OK", 0);
	delay_ms(10);
	HC05_Set_Cmd("AT+BLEGATTSSRVSTART\r\n", "OK", 0);
	delay_ms(10);
	HC05_Set_Cmd("AT+BLEADVSTART\r\n", "OK", 0);
	delay_ms(10);
	HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n", "OK", 0);
	delay_ms(10);
	BLE_SERVICE_STATUS = 1;
	memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
	USART3_RX_STA = 0;
	return InitRes;
}

/*************************************************
  Function:       // BlueToothMsgManage
  Description:    // 当蓝牙已开启，但没有设备接入时，根据log判断当前的连接状态，并进行对应的函数处理
  Input:          // atstr：当前的log
  Return:         // 设置结果
  Others:         // 结果（0：成功 ，1：失败）
*************************************************/
u8 BlueToothMsgManage(u8* atstr, u8 LenthOfAT)
{
	char* disconnloc;
	char* connloc;
	char* writeloc;
	char* paramloc;
	disconnloc = strstr(USART3_RX_BUF, "BLEDISCONN");
	connloc = strstr(USART3_RX_BUF, "BLECFGMTU:");
	writeloc = strstr(USART3_RX_BUF, "WRITE");
	paramloc = strstr(USART3_RX_BUF, "CONNPARAM");
	u1_printf("status+ %x ", BLE_SERVICE_STATUS);
	u1_printf("come into msgmanage");
	u1_printf(USART3_RX_BUF);
	u1_printf("lenof+ %x ", LenthOfAT);
	if (disconnloc != 0)
	{
		BLE_SERVICE_STATUS = 0;
		HC05_Set_Cmd("+++", "", 0);
		delay_ms(1000);
		HC05_Set_Cmd("AT+BLEADVSTART\r\n", "OK", 2);
		HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n", "OK", 2);
		BLE_SERVICE_STATUS = 1;
		return 0;
	}
	else if (connloc != 0 || paramloc != 0)
	{
		BLE_SERVICE_STATUS = 2;
		memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
		USART3_RX_STA = 0;
		return 0;
	}
	else if (writeloc != 0)
	{
		BLE_SERVICE_STATUS = 0;
		HC05_Set_Cmd("AT+BLESPPCFG=1,1,6,1,5\r\n", "OK", 2);
		delay_ms(10);
		HC05_Set_Cmd("AT+BLESPP\r\n", "OK", 2);
		memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
		BLE_SERVICE_STATUS = 3;
		USART3_RX_STA = 0;
	}
	else {
		if (LenthOfAT <= 5)
		{
			BLE_SERVICE_STATUS = 0;
			HC05_Set_Cmd("AT+BLEINIT=2\r\n", "", 0);
			delay_ms(10);
			HC05_Set_Cmd("AT+CWMODE=0\r\n", "OK", 0);
			delay_ms(10);
			HC05_Set_Cmd("AT+SYSMSG=7\r\n", "OK", 0);
			delay_ms(10);
			HC05_Set_Cmd("AT+BLEGATTSSRVCRE\r\n", "OK", 0);
			delay_ms(10);
			HC05_Set_Cmd("AT+BLEGATTSSRVSTART\r\n", "OK", 0);
			delay_ms(10);
			HC05_Set_Cmd("AT+BLEADVSTART\r\n", "OK", 0);
			delay_ms(10);
			HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n", "OK", 0);
		}
		BLE_SERVICE_STATUS = 1;
		USART3_RX_STA = 0;
		return 0;
	}
}

/*************************************************
  Function:       // BlueToothMsgManage
  Description:    // 当蓝牙有设备接入，但没有进入透传模式时，根据log判断当前的连接状态，并进行对应的函数处理
  Input:          // atstr：当前的log
  Return:         // 设置结果
  Others:         // 结果（0：成功 ，1：失败）
*************************************************/
u8 BlueToothSPPManage(u8* atstr)
{
	char* disconnloc;
	char* writeloc;
	u1_printf("status+ %x ", BLE_SERVICE_STATUS);
	u1_printf("come into sppmanage");
	u1_printf(USART3_RX_BUF);
	disconnloc = strstr(USART3_RX_BUF, "BLEDISCONN");
	writeloc = strstr(USART3_RX_BUF, "WRITE");
	if (disconnloc != 0)
	{
		HC05_Set_Cmd("+++", "", 0);
		BLE_SERVICE_STATUS = 0;
		delay_ms(1000);
		HC05_Set_Cmd("AT+BLEADVSTART\r\n", "OK", 2);
		HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n", "OK", 2);
		BLE_SERVICE_STATUS = 1;
		memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
		USART3_RX_STA = 0;
	}
	else if (writeloc != 0)
	{
		BLE_SERVICE_STATUS = 0;
		HC05_Set_Cmd("AT+BLESPPCFG=1,1,6,1,5\r\n", "OK", 2);
		delay_ms(10);
		HC05_Set_Cmd("AT+BLESPP\r\n", "OK", 2);
		memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
		BLE_SERVICE_STATUS = 3;
		USART3_RX_STA = 0;
	}
	return 0;
}

/*************************************************
  Function:       // BlueToothMsgManage
  Description:    // 当蓝牙进入透传模式时，根据log判断当前的连接状态，并进行对应的函数处理
  Input:          // atstr：当前的log
  Return:         // 设置结果
  Others:         // 结果（0：成功 ，1：失败）
*************************************************/
u8 BlueToothCmdManage(u8* atstr)
{
	u8 InitRes;
	char* disconnloc;
	char* writeloc;
	u1_printf("status+ %x ", BLE_SERVICE_STATUS);
	u1_printf("come into CMDmanage");
	u1_printf(USART3_RX_BUF);
	disconnloc = strstr(USART3_RX_BUF, "BLEDISCONN");
	writeloc = strstr(USART3_RX_BUF, "WRITE:");
	if (disconnloc != 0)
	{
		HC05_Set_Cmd("+++", "", 0);
		BLE_SERVICE_STATUS = 0;
		delay_ms(1000);
		HC05_Set_Cmd("AT+BLEADVSTART\r\n", "OK", 2);
		HC05_Set_Cmd("AT+BLEADVDATA=\"02010606094C5F6C6564030302A0\"\r\n", "OK", 2);
		BLE_SERVICE_STATUS = 1;
		memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
		USART3_RX_STA = 0;
		//free()
		return 0;
	}
	else if (writeloc != 0)
	{
		BLE_SERVICE_STATUS = 0;
		HC05_Set_Cmd("AT+BLESPPCFG=1,1,6,1,5\r\n", "OK", 2);
		delay_ms(10);
		HC05_Set_Cmd("AT+BLESPP\r\n", "OK", 2);
		memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
		BLE_SERVICE_STATUS = 3;
		USART3_RX_STA = 0;
	}
	else USART3_RX_STA = 0;
}



/*************************************************
  Function:       // BlueToothMsgManage
  Description:    // ATK-HC05蓝牙模块设置命令发送
  Input:          // atstr:AT指令串.比如:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"等字符串
  Return:         // 设置结果
  Others:         // 结果（0：成功 ，1：失败）
*************************************************/
u8 HC05_Set_Cmd(u8* atstr, u8* check, int spacecount)
{
	char* loc;
	u8 retry = 0X05;
	u8 temp, t;
	memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
	u1_printf("setcmd:%s", atstr);
	while (retry--)
	{
		delay_ms(10);
		u3_printf(atstr, 0);	//发送AT字符串
		for (t = 0; t < 20; t++) 			//最长等待100ms,来接收HC05模块的回应
		{
			delay_ms(10);
			if (USART3_RX_STA & 0X8000)break;
		}
		if ((USART3_RX_STA & 0X8000) > 0x0001)	//接收到一次数据了
		{
			temp = USART3_RX_STA & 0X7FFF;	//得到数据长度
			USART3_RX_STA = 0;
			loc = strstr(USART3_RX_BUF, check);
			if (loc != 0)
			{
				temp = 0;//接收到OK响应
				break;
			}
			else continue;
		}
	}
	//////需要修复
	if (retry == 0)
	{
		u1_printf("cmd retry 0");
		return 0;
	}//设置失败.
	u1_printf("receive cmd:%s", USART3_RX_BUF);
	return 1;
}











