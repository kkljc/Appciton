/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include "TrendWidget.h"
#include "SPO2BarWidget.h"
#include <QLabel>

class SPO2TrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置SPO2的值。
    void setSPO2Value(int16_t spo2);

    // 设置棒图的值。
    void setBarValue(int16_t bar);

    // 显示搜索脉搏的提示信息。
    void setSearchForPulse(bool isSearching);

    // 是否报警
    void isAlarm(bool flag);

    // 显示参数值
    void showValue(void);

    SPO2TrendWidget();
    ~SPO2TrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_spo2Value;
    SPO2BarWidget *_spo2Bar;
    QString _spo2String;
    bool _isAlarm;
};
