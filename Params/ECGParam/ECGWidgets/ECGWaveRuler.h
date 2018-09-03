/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/27
 **/


#pragma once
#include <QPoint>
#include "WaveWidgetItem.h"
#include "ECGDefine.h"

class ECGWaveWidget;
/***************************************************************************************************
 * ECG 波形控件标尺
 **************************************************************************************************/
class ECGWaveRuler : public WaveWidgetItem
{
public:
    explicit ECGWaveRuler(ECGWaveWidget *wave);
    ~ECGWaveRuler();

    void paintItem(QPainter &painter);
    void setGain(ECGGain g);

private:
    ECGGain _gain;
    float _pixelHPitch;
    QWidget *_waveWidget;
};
