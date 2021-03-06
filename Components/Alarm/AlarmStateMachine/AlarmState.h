/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/3
 **/


#pragma once
#include <QObject>
#include "AlarmStateDefine.h"

//报警状态
class AlarmState : public QObject
{
    Q_OBJECT

public:
    explicit AlarmState(ALarmStateType type = ALARM_STATE_NONE);
    virtual ~AlarmState() {}

    ALarmStateType type() const {return _type;}

    void beginTimer(int interval);
    void endTimer();
    int getTimerID() const {return _timerID;}

public:
    // 状态进入/退出
    virtual void enter();
    virtual void exit();

    // 定时器处理
    virtual void timerEvent(QTimerEvent *e);

    // 事件处理
    virtual void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);

private:
    ALarmStateType _type;
    int _timerID;
};

