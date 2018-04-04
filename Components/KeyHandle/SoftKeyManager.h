#pragma once
#include "IWidget.h"
#include "SystemDefine.h"
#include "SoftkeyActionBase.h"
#include <QMap>

class SoftkeyWidget;
class SoftKeyManager : public IWidget
{
    Q_OBJECT

public:
    static SoftKeyManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SoftKeyManager();
        }
        return *_selfObj;
    }
    static SoftKeyManager *_selfObj;
    ~SoftKeyManager();

public:
    // 界面类型到软按键类型的转换
    SoftKeyActionType uFaceTypeToSoftKeyType(UserFaceType type);

    // 根据界面类型显示不同的内容。
    void setContent(SoftKeyActionType type, bool reload = false);

    // 前面板按键按下1-5。
    void softKeyPress(int index);
    void softkeyReleased(int index);

    // 切换到指定的动作组。
    void switchTo(SoftKeyActionType type);

    // 切换到上一个page。
    void previousPage(void);

    // 切换到下一个page。
    void nextPage(void);

    // 返回页码信息。
    int returnPage(void);

    // 切换到首页
    bool returnRootPage(void);

    // 刷新当前页。
    void refresh(bool toFirstPage = false);

    // 获取软按键动作对象。
    SoftkeyActionBase *getAction(SoftKeyActionType t);

    virtual void getSubFocusWidget(QList<QWidget*> &/*subWidget*/) const;

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void _clickKey(IWidget *w);

private:
    SoftKeyManager();
    void _layoutKeys(void);

    typedef QMap<SoftKeyActionType, SoftkeyActionBase*> ActionMap;
    ActionMap _actions;
    SoftkeyActionBase *_currentAction;
    SoftKeyActionType _curSoftKeyType;

    int _totalPages;      // 总共的页数。
    int _currentPage;     // 当前页。

    int _KEY_SIZE_H;                             //按钮的高度
    static const int SOFTKEY_WIDGET_NR = 12;      //按钮显示的数量
    QList<SoftkeyWidget*> _keyWidgets;
};
#define softkeyManager (SoftKeyManager::construction())