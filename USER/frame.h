#ifndef __FRAME_H
#define __FRAME_H	 
#include "sys.h"  


#define  FrameHead1 0x55
#define  FrameHead2 0xaa


u8 FrameCheckAndDataSet(u8 FrameData[],u8 Framelen);
u8 CheckSum(u8 FrameData[],u8 Framelen);
u8 Datacmd(u8 FrameCmd[]);
u8 GetReadyToOTA();  
u8 LoadOTAData(u8 FrameData[]);
u8 ReturnFrame(u8 FrameCmd ,u8 DataLenth,u8 ResData[]);

#endif  