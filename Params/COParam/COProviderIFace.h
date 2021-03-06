/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/8
 **/

#pragma once
#include "PatientDefine.h"
#include "CODefine.h"
#include <QString>

// 定义CO数据提供对象需要实现的接口方法。
class COProviderIFace
{
public:
    virtual ~COProviderIFace() { }
    // CO 测量控制
    virtual void measureCtrl(COMeasureCtrl /*instctl*/){}

    // CO 测量时间间隔设置
    virtual void setMeasureInterval(COMeasureInterval /*interval*/){}

    // Ti 输入模式设置, need ti value when in manual mode
    virtual void setTiSource(COTiSource /*inputmode*/, unsigned short /*ti*/){}

    // 注射液体积设定
    virtual void setInjectionVolume(unsigned char /*volume*/){}

    // 漂浮导管系数设定, x1000
    virtual void setCatheterCoeff(unsigned short /*ratio*/){}

    /* get the board type */
    virtual COModuleType getCOModuleType() { return CO_MODULE_NR; }

    /**
     * @brief getMeasureWaveRate the upload measure wave rate
     * @return the wave data rate
     */
    virtual short getMeasureWaveRate() { return 25;}

    /**
     * @brief isTiSensorOff get the ti sensor status
     * @return true when sensor off, otherwise, false
     */
    virtual bool isTiSensorOff() const { return false;}

    /**
     * @brief isTbSensorOff get the tb sensor status
     * @return true when sensor off, otherwise, false
     */
    virtual bool isTbSensorOff() const { return false;}
};
