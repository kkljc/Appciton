#pragma once

typedef short TrendDataType;
typedef int WaveDataType;

enum WaveFlagBit
{
    ECG_INTERNAL_FLAG_BIT = 1,                  // 内部起搏
    ECG_EXTERNAL_SOLD_FLAG_BIT = (1 << 1),      // 外部起搏(实线)
    INVALID_WAVE_FALG_BIT = (1 << 14),           // 无效波形
    ECG_WAVE_RATE_250_BIT = (1 << 15)           // 波形采样速率为250/S
};

#define InvData()     (-5000)
#define InvStr()      ("---")
#define EmptyStr()    ("")

#define UnknownData() (-8000)
#define UnknownStr()  ("???")

#define MAX_PATIENT_ID_LEN      (30 + 1)
#define MAX_PATIENT_NAME_LEN    (30 + 1)

#define MAX_TIME_LEN      (32)    // 单位:秒
#define MAX_SAMPLE_RATE   (250)   // 单位:个
#define MAX_WAVEFORM_CACHE_LEN \
    (MAX_TIME_LEN * MAX_SAMPLE_RATE * sizeof(WaveDataType))    // 单位:字节

#define MAX_SUMMARY_TITLE_LEN (64)       // Summary名称长度
#define MAX_SUMMARY_DATA_LEN \
    (MAX_WAVEFORM_CACHE_LEN + MAX_SUMMARY_TITLE_LEN + 512)   // Summary数据长度

#define MAX_DEVICE_ID_LEN (11 + 1) //设备标识符长度
#define MAX_SERIAL_NUM_LEN (11 + 1)