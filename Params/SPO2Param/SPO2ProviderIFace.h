/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/8/13
 **/

#pragma once
#include "SPO2Define.h"

// 定义ECG数据提供对象需要实现的接口方法。
class SPO2ProviderIFace
{
public:
    // 设置灵敏度。
    virtual void setSensitive(SPO2Sensitive sens) = 0;

    // 查询状态。
    virtual void sendStatus(void) { }

    // 获取波形的采样速度。
    virtual int getSPO2WaveformSample(void) = 0;

    // 获取波形基线
    virtual int getSPO2BaseLine(void) = 0;

    // 取值范围
    virtual int getSPO2MaxValue(void) = 0;

    // SPO2值与PR值。
    virtual bool isResult_SPO2PR(unsigned char */*packet*/) {return false;}

    // 描记波
    virtual bool isResult_BAR(unsigned char */*packet*/) {return false;}

    // 状态
    virtual bool isStatus(unsigned char */*packet*/) {return false;}


    // 发送协议命令
    virtual void sendCmdData(unsigned char /*cmdId*/, const unsigned char */*data*/,
                             unsigned int /*len*/) { }

    // 析构。
    virtual ~SPO2ProviderIFace() { }

    /**
     * @brief initModule  and the function of initting the module
     */
    virtual void initModule() { }

public:
    // Masimo

    // 设置灵敏度和FastSat。
    virtual void setSensitivityFastSat(SensitivityMode /*mode*/, bool /*fastSat*/) { }

    // 设置平均时间。
    virtual void setAverageTime(AverageTime /*mode*/) { }

    // 设置SMart Tone使能选项。
    virtual void setSmartTone(bool /*enable*/) { }

    /**
     * @brief setSpHbPrecisionMode 设置SpHb精确度
     * @param mode
     */
    virtual void setSpHbPrecisionMode(SpHbPrecisionMode /*mode*/){}

    /**
     * @brief setPVIAveragingMode 设置PVI的平均速度
     * @param mode
     */
    virtual void setPVIAveragingMode(AveragingMode /*mode*/){}

    /**
     * @brief setSpHbBloodVesselMode 设置SpHb血管模式（动脉/静脉）
     * @param mode
     */
    virtual void setSpHbBloodVesselMode(SpHbBloodVesselMode /*mode*/){}

    /**
     * @brief setSphbAveragingMode 设置sphb平均速度
     * @param mode
     */
    virtual void setSphbAveragingMode(SpHbAveragingMode /*mode*/){}

    /**
     * @brief setLineFrequency set the line frequency, rainbow only
     * @param freq
     */
    virtual void setLineFrequency(SPO2LineFrequencyType /*freq*/){}

    /**
     * @brief setProgramResponse set the program reqeust response
     * @note
     * For rainbow only
     */
    virtual void setProgramResponse(bool /*ack*/){}

    // Nellcor
    /**
     * @brief setSatSeconds 设置过高低限值的容忍时间
     * @param type 时间类型
     */
    virtual void setSatSeconds(Spo2SatSecondsType /*type*/){}

    /**
     * @brief setRawDataSend 设置原始数据发送开关
     */
    virtual void enableRawDataSend(bool /*onOff*/){}
};
