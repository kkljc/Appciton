/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/6
 **/

#pragma once
#include "Window.h"
#include <QScopedPointer>
#include <QModelIndex>
#include "ParamDefine.h"

class OxyCRGEventWindowPrivate;
class OxyCRGEventWindow : public Window
{
    Q_OBJECT
public:
    static OxyCRGEventWindow &constraction()
    {
        if (NULL == selfObj)
        {
            selfObj = new OxyCRGEventWindow();
        }
        return *selfObj;
    }
    ~OxyCRGEventWindow();

    void waveWidgetTrend1(bool isRR);
    void waveWidgetCompressed(WaveformID id);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void waveInfoReleased(QModelIndex index);
    void eventListReleased(void);
    void leftMoveEvent(void);
    void rightMoveEvent(void);
    void printReleased(void);
    void setReleased(void);

private:
    OxyCRGEventWindow();
    static OxyCRGEventWindow *selfObj;
    QScopedPointer<OxyCRGEventWindowPrivate> d_ptr;
};
#define oxyCRGEventWindow               (OxyCRGEventWindow::constraction())
#define deleteOxyCRGEventWindow         (delete OxyCRGEventWindow::selfObj)
