/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/

#pragma once
#include "MenuContent.h"

class NIBPZeroPointContentPrivate;
class NIBPZeroPointContent : public MenuContent
{
    Q_OBJECT

public:
    static NIBPZeroPointContent *getInstance();
    ~NIBPZeroPointContent();

protected:
    virtual void layoutExec(void);
    void timerEvent(QTimerEvent *ev);
    bool focusNextPrevChild(bool next);

private slots:
    /**
     * @brief enterZeroReleased 进入/退出校零模式
     */
    void enterZeroReleased(void);

    void pumpControlReleased(void);

    void valveControlReleased(void);

    void calibrateZeroReleased(void);

private:
    NIBPZeroPointContent();

    NIBPZeroPointContentPrivate *const d_ptr;
};



