#pragma once
#include "ECGDefine.h"
#include "SystemDefine.h"

#define WAVE_SAMPLE_RATE_250    (250)
#define WAVE_SAMPLE_RATE_500    (500)

// 定义ECG数据提供对象需要实现的接口方法。
class ECGProviderIFace
{
public:
    // 获取波形的采样速度。
    virtual int getWaveformSample(void) = 0;

    // 设置波形的采样速度。
    virtual void setWaveformSample(int rate) = 0;

    virtual int getBaseLine(void) = 0;

    // 获取+/-0.5mV对应的数值。
    virtual void get05mV(int &p05mv, int &n05mv) = 0;

    virtual void getLeadCabelType() = 0;

    // 设置导联系统。
    virtual void setLeadSystem(ECGLeadMode leadSystem) = 0;

    // 设置计算导联。
    virtual void setCalcLead(ECGLead lead) = 0;

    // 设置病人类型。
    virtual void setPatientType(unsigned char type) = 0;

    // 设置带宽。
    virtual void setBandwidth(ECGBandwidth bandwidth) = 0;

    // set filter mode
    virtual void setFilterMode(ECGFilterMode mode) = 0;

    // 起搏器设置。
    virtual void enablePacermaker(ECGPaceMode onoff) = 0;

    // 设置工频滤波。
    virtual void setNotchFilter(ECGNotchFilter lotch) = 0;

    // ST开关。
    virtual void enableSTAnalysis(bool onoff) = 0;

    // ST点设置。
    virtual void setSTPoints(int iso, int st) = 0;

    // 使能Pace sync控制
    virtual void enablePaceSyncCtrl(bool /*enable*/) {}

    // 使能PD Blank控制
    virtual void enablePDBlankCtrl(bool /*enable*/) {}

    // enable data sync control
    virtual void enableDataSyncCtrl(bool /*enable*/) {}

    // enable vf calc control
    virtual void enableVFCalcCtrl(bool /*enable*/) {}

    //发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/,
                             unsigned int /*len*/) { }

    ECGProviderIFace() { }
    virtual ~ECGProviderIFace() { }
};