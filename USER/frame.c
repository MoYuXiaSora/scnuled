#include "delay.h"
#include "usart.h"
#include "usart3.h"
#include "hc05.h"
#include "string.h"
#include "math.h"
#include "frame.h"
#include "CRC.h"

unsigned long CRC32_Caledse;                                             // �����CRC32ֵ
u8 CorrectFrame[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // ��ȷ�������ص�����֡
u8 WrongFrame[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }; // ���������ص�����֡

// ������������ĸ������
typedef struct OtaFile {
    unsigned char title[20];
    unsigned long lenth;
    unsigned long CRC32_Caled;
    unsigned long FileCRC32;
    unsigned char data[60000];
    u16 PacketOffSet; // ��ǰ�ļ��İ�ƫ��
} OtaFile1;

static struct OtaFile Otafile1;

u8 FrameCheckAndDataSet(u8 FrameData[], u8 FrameLen)
{
    u16 FrameHead = ((0x0000 | (FrameData[0] << 8)) | FrameData[1]); // ���֡ͷ
    u8 FrameCmd = FrameData[2];
    if (FrameLen <= 5) {
        memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
        USART3_RX_STA = 0;
        return 0;
    }
    if (FrameHead != 0x55aa) {
        // ֡ͷ����Ļ�һ����ATָ�� ����ATָ�������
        BlueToothCmdManage(USART3_RX_BUF);
        return 0;
    }
    // ���ݵ���λ���жϣ�����֡
    switch (FrameCmd) {
        case 0x01:
            if (CheckSum(FrameData, FrameLen) != FrameData[FrameLen - 1]) {
                ReturnFrame(0xff, 0x01, WrongFrame); // send error msg
                memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
                USART3_RX_STA = 0;
                break;
            }
            ReturnFrame(0x01, 0x08, "00010101");
            memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
            USART3_RX_STA = 0;
            break;
        case 0x02:
            if (CheckSum(FrameData, FrameLen) != FrameData[FrameLen - 1]) {
                ReturnFrame(0xff, 0x01, WrongFrame);
                memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
                USART3_RX_STA = 0;
                break;
            }
            // ʵʱ�����·�����
            Datacmd(FrameData);
            memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
            USART3_RX_STA = 0;
            break;
        case 0x05:
            if (CheckSum(FrameData, FrameLen) != FrameData[FrameLen - 1]) {
                ReturnFrame(0x05, 0x01, WrongFrame);
                // send error msg
                memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
                USART3_RX_STA = 0;
                break;
            }
            GetReadyToOTA(FrameData);

            // TIM_Cmd(TIM6,ENABLE);
            memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
            USART3_RX_STA = 0;
            break;
        case 0x06:
            if (CheckSum(FrameData, FrameLen) != FrameData[FrameLen - 1]) {
                ReturnFrame(0x06, 0x01, WrongFrame);
                memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
                USART3_RX_STA = 0;
                break;
            }
            LoadOTAData(FrameData);
            memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
            USART3_RX_STA = 0;
            break;
        case 0xee:
            if (CheckSum(FrameData, FrameLen) != FrameData[FrameLen - 1]) {
                ReturnFrame(0xff, 0x01, WrongFrame);
                memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
                USART3_RX_STA = 0;
                // send error msg
                break;
            }
            CheckFinalRes();
            memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
            USART3_RX_STA = 0;
            break;
    }

    memset(USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
    USART3_RX_STA = 0;
}

u8 Datacmd(u8 FrameCmd[])
{
    u8 SleepCmd = FrameCmd[5];
    u16 HIS_HUE = (0x0000 | (FrameCmd[6] << 8 | FrameCmd[7]));
    u8 HIS_S = FrameCmd[8];
    u8 HIS_I = FrameCmd[9];
    u16 CCT_CT = (0x0000 | (FrameCmd[10] << 8 | FrameCmd[11]));
    u8 CCT_I = FrameCmd[12];
    u8 FX_MODE = FrameCmd[13];
    u8 FX_TYPE = FrameCmd[14];
    u8 FX_I = FrameCmd[15];
    u8 FX_FREQ = FrameCmd[16];
    // ����������
    return 0;
}

u8 GetReadyToOTA(u8 FrameData[])
{
    char FileTitle[10] = { "XJBGJ" };
    init_CRC32_table();
    memcpy(Otafile1.title, FileTitle, sizeof(FileTitle));
    Otafile1.lenth = (0x00000000 | (FrameData[5] << 24) | (FrameData[6] << 16) | (FrameData[7] << 8) | (FrameData[8]));
    Otafile1.FileCRC32 =
        (0x00000000 | (FrameData[9] << 24) | (FrameData[10] << 16) | (FrameData[11] << 8) | (FrameData[12]));
    Otafile1.PacketOffSet = 0;
    Otafile1.CRC32_Caled = 0xffffffff;
    memset(Otafile1.data, 0, 60000); //
    u1_printf("0x05");
    // ����ota����
    // memcpy (Otafile1.data,FileData,Otafile1.lenth);
    ReturnFrame(0x05, 0x01, CorrectFrame);
    return 0;
}
u8 CheckFinalRes(void)
{
    if (Otafile1.PacketOffSet == Otafile1.lenth && ~Otafile1.CRC32_Caled == Otafile1.FileCRC32) { // ���һ����
        u1_printf("0x06");
        ReturnFrame(0x06, 0x01, CorrectFrame);
    } else {
        ReturnFrame(0x06, 0x01, WrongFrame);
    }
    return 0;
}

u8 LoadOTAData(u8 FrameData[])
{
    u32 DevicePacketOffset =
        (0x00000000 | (FrameData[5] << 24) | (FrameData[6] << 16) | (FrameData[7] << 8) | (FrameData[8]));
    u8 DataLen = FrameData[4] - 4;
    if (DevicePacketOffset < Otafile1.PacketOffSet) { // ������һ�������ش�ֱ�ӻظ�
        ReturnFrame(0x06, 0x01, CorrectFrame);
        return 0;
    }
    if (DevicePacketOffset == Otafile1.PacketOffSet) {
        Otafile1.PacketOffSet += DataLen;
        // memcpy(frame2.data, FrameData+9, DataLen);
        memcpy(Otafile1.data + DevicePacketOffset, FrameData + 9, DataLen);
        Otafile1.CRC32_Caled = GetCRC32(Otafile1.CRC32_Caled, FrameData + 9, DataLen);
        if (Otafile1.PacketOffSet == Otafile1.lenth) {
            if (~Otafile1.CRC32_Caled == Otafile1.FileCRC32) { // ���һ����
                ReturnFrame(0x06, 0x01, CorrectFrame);
                u1_printf("0x06");
            } else {
                Otafile1.PacketOffSet -= DataLen;
                ReturnFrame(0x06, 0x01, WrongFrame);
            }
            return 0;
        }
        ReturnFrame(0x06, 0x01, CorrectFrame);
        return 0;
    }
    ReturnFrame(0x06, 0x01, WrongFrame);
    return 0;
}

u8 CheckSum(u8 FrameData[], u8 Framelen)
{
    u16 FrameSum = 0x0000;
    int res;
    for (res = 0; res <= Framelen - 2; res++) {
        FrameSum += FrameData[res];
    }
    return FrameSum % 256;
}


// ����֡ 0�ɹ� 1ʧ��
u8 ReturnFrame(u8 FrameCmd, u8 DataLenth, u8 ResData[8])
{
    u8 ResFrame[80];
    int res;
    u16 FrameSum = 0x0000;
    memset(ResFrame, 0, 14);
    ResFrame[0] = 0x55;
    ResFrame[1] = 0xaa;
    ResFrame[2] = FrameCmd;
    ResFrame[3] = 0x00;
    ResFrame[4] = DataLenth;
    memcpy(ResFrame + 5, ResData, 8);
    for (res = 0; res < 13; res++) {
        FrameSum += ResFrame[res];
    }
    ResFrame[13] = (FrameSum) % 256;
    u3_printf(ResFrame, 14);
    return 0;
}
