/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#pragma once
#include "MenuContent.h"

class AlarmLimitMenuContentPrivate;

class AlarmLimitMenuContent : public MenuContent
{
    Q_OBJECT
public:
    AlarmLimitMenuContent();
    ~AlarmLimitMenuContent();

    /* reimplement */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onbtnClick();

private:
    AlarmLimitMenuContentPrivate *const d_ptr;
};