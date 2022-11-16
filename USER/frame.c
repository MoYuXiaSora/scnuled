#include "delay.h" 			 
#include "usart.h" 			 
#include "usart3.h" 			 
#include "hc05.h"  
#include "string.h"	 
#include "math.h"
#include "frame.h"
#include "CRC.h"

u8 OTAPacketLenth;  //升级主控板返回的每个包携带的数据长度 目前都为10
typedef struct frame {
	u16 head;
	u8 cmd;
	u8 packet_num1;
	u8 packet_num2;
	u16 packet_len;
	u8 packet_offset;
	u8 filesize;
	u8 crc;
	u8 checksum;
	u8 data[10]; 
};
//static
typedef struct OtaFile {
	unsigned char title[20];
	unsigned long lenth;
	unsigned long CRC32_Caled;
	unsigned long FileCRC32;
	unsigned char data[60000];
	u16 PacketOffSet;  //当前文件的包偏移
}OtaFile1;

static struct OtaFile Otafile1;
//先检察帧头 检查之后根据对应的命令字放进处理的函数中		
u8 FrameCheckAndDataSet(u8 FrameData[],u8 Framelen){
	u16 FrameHead = ((0x0000|(FrameData[0]<<8))|FrameData[1]);//检查帧头
	u8 FrameCmd = FrameData[2];
	if(FrameHead!=0x55aa){
			BlueToothCmdManage(USART3_RX_BUF);
			USART3_RX_STA=0 ;
		//帧头错误的话一般是AT指令 交给AT指令处理函数
	}
	   //开启定时器
	//改成正则表达式？
	//根据第三位先判断，再组帧
		switch (FrameCmd){
		case 0x01:
			if(CheckSum(FrameData,Framelen)!=FrameData[Framelen-1]){
			ReturnFrame(0xff,0x01,"00000001");//send error msg
			memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
      USART3_RX_STA=0;
				break;
			}
			ReturnFrame(0x01,0x08,"00010101");
			memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
      USART3_RX_STA=0;
		  break;
			
			
		case 0x02:
			if(CheckSum(FrameData,Framelen)!=FrameData[Framelen-1]){
			ReturnFrame(0xff,0x01,"00000001");
				//send error msg
			memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
      USART3_RX_STA=0;
				break;
			}
			//实时数据下发处理
			Datacmd(FrameData);
			memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
      USART3_RX_STA=0;
		  break;	
			
			
		case 0x05:
		  if(CheckSum(FrameData,Framelen)!=FrameData[Framelen-1]){
			ReturnFrame(0xff,0x01,"00000001");
				//send error msg
			memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
      USART3_RX_STA=0;
				break;
			}
			GetReadyToOTA(FrameData);
			
			//TIM_Cmd(TIM6,ENABLE);
			memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
      USART3_RX_STA=0;
			break;
			
			
		case 0x06:
			if(CheckSum(FrameData,Framelen)!=FrameData[Framelen-1]){
				ReturnFrame(0xff,0x01,"00000001");
				memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
        USART3_RX_STA=0;
				//send error msg
				break;
			}
			LoadOTAData(FrameData);
		  memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
      USART3_RX_STA=0;
			break;
	}
    memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);
    USART3_RX_STA=0;		
}

u8 Datacmd(u8 FrameCmd[]){
	u8 SleepCmd = FrameCmd[5];
	u16 HIS_HUE = (0x0000|(FrameCmd[6]<<8|FrameCmd[7]));
	u8 HIS_S = FrameCmd[8];
	u8 HIS_I = FrameCmd[9];
	u16 CCT_CT = (0x0000|(FrameCmd[10]<<8|FrameCmd[11]));
	u8 CCT_I =  FrameCmd[12];
	u8 FX_MODE =  FrameCmd[13];
	u8 FX_TYPE =  FrameCmd[14];
	u8 FX_I =  FrameCmd[15];
	u8 FX_FREQ =  FrameCmd[16];
	//后续待处理
	return 0 ;
}

u8 GetReadyToOTA(u8 FrameData[] ){	
	char FileTitle [10] = {"XJBGJ"};	
	u8 CorrectFrame[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	init_CRC32_table();
	memcpy(Otafile1.title, FileTitle, sizeof(FileTitle));
	Otafile1.lenth = (0x00000000 | (FrameData[5] << 24) | (FrameData[6] << 16) | (FrameData[7] << 8) | (FrameData[8]));
	Otafile1.FileCRC32 = (0x00000000 | (FrameData[9] << 24) | (FrameData[10] << 16) | (FrameData[11] << 8) | (FrameData[12]));
	Otafile1.PacketOffSet = 0 ;
	Otafile1.CRC32_Caled = 0xffffffff;
	memset(Otafile1.data,0,12000);
	u1_printf("0x05");
	//进行ota复制
	//memcpy (Otafile1.data,FileData,Otafile1.lenth);
	ReturnFrame(0x05,0x01,CorrectFrame);
	return 0 ;
}

u8 LoadOTAData(u8 FrameData[]) {
	struct frame frame2;
	u8 CorrectFrame[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	u32 DevicePacketOffset = (0x00000000|(FrameData[5]<<24)|(FrameData[6]<<16)|(FrameData[7]<<8)|(FrameData[8]));
	u8 DataLen = FrameData[4]-4;
	frame2.packet_len = (0x00000000|(FrameData[3]<<8)|FrameData[4])-4;	//数据长度
  if (DevicePacketOffset<Otafile1.PacketOffSet)//就是上一个包的重传
		{ //直接回复
			ReturnFrame(0xff,0x01,CorrectFrame);
			return 0 ;
	}	
	if (DevicePacketOffset==Otafile1.PacketOffSet){
		Otafile1.PacketOffSet+=DataLen;
		memcpy(frame2.data, FrameData+9, DataLen);
		memcpy(Otafile1.data+DevicePacketOffset,FrameData+9,DataLen);
		Otafile1.CRC32_Caled = GetCRC32(Otafile1.CRC32_Caled, frame2.data , DataLen);
		ReturnFrame(0xff,0x01,CorrectFrame);
		if(Otafile1.PacketOffSet==Otafile1.lenth){//最后一个包	
			if((~Otafile1.CRC32_Caled)==Otafile1.FileCRC32){
				u1_printf("0x06 last");
				delay_ms(100);
				ReturnFrame(0x06,0x01,CorrectFrame);
			}
			//计算CRC32
		}  
	}	
}

u8 CheckSum(u8 FrameData[],u8 Framelen){
	u16 FrameSum = 0x0000;
	int res ;
	for(res = 0; res<=Framelen-2; res++ ){
		FrameSum+=FrameData[res];
	}
	return FrameSum%256;
}	  


//返回帧 0成功 1失败
u8 ReturnFrame(u8 FrameCmd ,u8 DataLenth,u8 ResData[8]){
	u8 ResFrame[14];
	int res ;
	u16 FrameSum = 0x0000;
	memset(ResFrame,0,14);
	ResFrame[0] = 0x55;
	ResFrame[1] = 0xaa;
	ResFrame[2] = FrameCmd;
	ResFrame[3] = 0x00 ;
	ResFrame[4] = DataLenth;
	memcpy(ResFrame+5,ResData,8);
	for(res = 0; res<13; res++ ){
		FrameSum+=ResFrame[res];
	}
	ResFrame[13] = FrameSum%256;
  u3_printf(ResFrame,14)	;
	return 0;
}

u8 OTAFailCmd(){
	//memset()
  //升级失败流程
	u3_printf("OTA failed",0);
}



