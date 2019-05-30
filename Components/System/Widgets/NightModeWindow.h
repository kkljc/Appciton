/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/30
 **/

#pragma once
#include "Dialog.h"

class NightModeWindowPrivate;
class NightModeWindow : public Dialog
{
    Q_OBJECT
public:
    NightModeWindow();
    ~NightModeWindow();

    /**
     * @brief readyShow
     */
    void readyShow();

    /**
     * @brief layoutExec
     */
    void layoutExec();

protected:
    void hideEvent(QHideEvent *ev);

private slots:
    /**
     * @brief onComboBoxIndexChanged  下拉框选项改变槽函数
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief OnBtnReleased  按钮释放槽函数
     */
    void OnBtnReleased(void);

    /**
     * @brief onPopupListItemFocusChanged - the slot function when the focus of the popuplist item changed
     * @param volume: the volume
     */
    void onPopupListItemFocusChanged(int volume);

private:
    NightModeWindowPrivate *const d_ptr;
};

