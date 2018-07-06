/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#pragma once
#include <QDialog>
#include <QScopedPointer>

class WindowPrivate;
class Window : public QDialog
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = NULL);
    ~Window();

    QLayout *getWindowLayout();
    void setWindowLayout(QLayout *layout);

    /**
     * @brief showMask show the window mask or not
     * @param flag
     */
    void showMask(bool flag);

    /**
     * @brief isShowingMask check whether showing mask
     * @return
     */
    bool isShowingMask() const;

protected:
    /* reimplement */
    void changeEvent(QEvent *ev);
    /* reimplement */
    void resizeEvent(QResizeEvent *ev);

private:
    QScopedPointer<WindowPrivate> d_ptr;
};
