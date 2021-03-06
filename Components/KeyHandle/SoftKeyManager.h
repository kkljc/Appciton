/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/20
 **/


#pragma once
#include "IWidget.h"
#include "SystemDefine.h"
#include "SoftkeyActionBase.h"
#include <QMap>

class SoftkeyWidget;
class SoftKeyManagerPrivate;
class SoftKeyManager : public IWidget
{
    Q_OBJECT

public:
    static SoftKeyManager &getInstance(void);
    ~SoftKeyManager();

public:
    // 界面类型到软按键类型的转换
    SoftKeyActionType uFaceTypeToSoftKeyType(UserFaceType type);

    // 根据界面类型显示不同的内容。
    void setContent(SoftKeyActionType type);

    // 切换到指定的动作组。
    void switchTo(SoftKeyActionType type);

    // 获取软按键动作对象。
    SoftkeyActionBase *getAction(SoftKeyActionType t);

    virtual void getSubFocusWidget(QList<QWidget*> &/*subWidget*/) const;

    /**
     * @brief refreshPage 刷新软按键当前页
     * @param isFirstPage 是否恢复到第一页
     */
    void refreshPage(bool isFirstPage = true);

     /**
      * @brief setKeyTypeAvailable  设置按键类型是否可用
      * @param keyType  按键类型
      * @param isAvailable  是否有用
      */
     void setKeyTypeAvailable(SoftBaseKeyType keyType, bool isAvailable);

     void refreshCO2Key(bool on);

     /**
      * @brief refreshTouchKey 刷新触摸按键内容显示
      */
     void refreshTouchKey();

     /**
      * @brief setFocusBaseKey  设置焦点在快捷按键接口
      * @param keyType  按键类型
      */
     void setFocusBaseKey(SoftBaseKeyType keyType);

public slots:
    /**
     * @brief onUserFaceChanged handle the user interface layout change event
     */
    void onUserFaceChanged(UserFaceType type);

    /**
     * @brief setIBPZeroKeyAvailable  set IBP Zero key Available
     * @param available  available
     */
    void setIBPZeroKeyAvailable(bool available);

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:
    /* handle the dynamic key released signal */
    void _dynamicKeyReleased(int index);
    /* handle the dynamic key pressed signal */
    void _dynamicKeyPressed(int index);
        /* handle the fixed key clicked event */
    void _fixedKeyClicked();

private:
    SoftKeyManager();
    SoftKeyManagerPrivate * const d_ptr;
};
#define softkeyManager (SoftKeyManager::getInstance())
