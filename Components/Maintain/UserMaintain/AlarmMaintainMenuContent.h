/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class AlarmMaintainMenuContentPrivate;
class AlarmMaintainMenuContent : public MenuContent
{
    Q_OBJECT
public:
    AlarmMaintainMenuContent();
    ~AlarmMaintainMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    void onButtonReleased(void);

    /**
     * @brief onPopupListItemFocusChanged
     * @param volume
     */
    void onPopupListItemFocusChanged(int volume);

private:
    AlarmMaintainMenuContentPrivate * const d_ptr;
};
