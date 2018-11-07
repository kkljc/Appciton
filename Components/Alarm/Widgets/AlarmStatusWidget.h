/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/31
 **/


#pragma once
#include "IWidget.h"
#include "AlarmDefine.h"

class AlarmStatusWidget : public IWidget
{
    Q_OBJECT
public:
    /**
     * @brief setAlarmStatus set the alarm status
     * @param status new status
     */
    void setAlarmStatus(AlarmAudioStatus status);

    // 构造与析构。
    AlarmStatusWidget();
    virtual ~AlarmStatusWidget();

protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap _muteAlarmPause;            // 报警暂停图标。
    QPixmap _muteAudioOff;              // 报警静音图标。
    QPixmap _muteAlarmOff;              // 报警关闭图标。
    AlarmAudioStatus _audioStatus;
};