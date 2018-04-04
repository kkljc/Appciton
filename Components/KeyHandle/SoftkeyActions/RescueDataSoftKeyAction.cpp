#include <QApplication>
#include "RescueDataSoftKeyAction.h"
#include "WindowManager.h"
#include "TrendDataWidget.h"
#include "TrendGraphWidget.h"
#include "SummaryRescueDataWidget.h"
#include "SummaryRescueRangePrint.h"
#include "RescueDataExportWidget.h"
#include "IMessageBox.h"
#include "DataStorageDirManager.h"
#include "SoftKeyManager.h"
#include "MenuGroup.h"
#include "MenuManager.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _rescueDataKeys[] =
{
//    KeyActionDesc("", "Trend.png",   RescueDataSoftKeyAction::trendReview),
//    KeyActionDesc("", "Summary.png", RescueDataSoftKeyAction::summaryReview),
//    KeyActionDesc("", "Export.png",  RescueDataSoftKeyAction::exportData),
//    KeyActionDesc("", "Delete.png",  RescueDataSoftKeyAction::clearData),
//    KeyActionDesc("", "SoftkeyArrow.png", RescueDataSoftKeyAction::exit),

    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PREVIOUS_PAGE),
    KeyActionDesc("", "Trend.png",   RescueDataSoftKeyAction::trendReview),
    KeyActionDesc("", "Summary.png", RescueDataSoftKeyAction::summaryReview),
    KeyActionDesc("", "Export.png",  RescueDataSoftKeyAction::exportData),
    KeyActionDesc("", "Delete.png",  RescueDataSoftKeyAction::clearData),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, false, Qt::black, Qt::black, Qt::black, false),
    KeyActionDesc("", "SoftkeyArrow.png", RescueDataSoftKeyAction::exit),
    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_MAIN_SETUP),
};

/***************************************************************************************************
 * 趋势回顾回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::trendReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    bool isVisible = trendDataWidget.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - menuManager.width()) / 2;
    int y = r.y() + (r.height() - menuManager.height());
    trendDataWidget.autoShow(x, y);
}

/***************************************************************************************************
 * summary回顾回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::summaryReview(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

//    bool isVisible = summaryRescueDataWidget.isVisible();
    bool isVisible = trendGraphWidget.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - menuManager.width()) / 2;
    int y = r.y() + (r.height() - menuManager.height());
    //summaryRescueDataWidget.autoShow(x, y);
    trendGraphWidget.autoShow(x, y);
}

/***************************************************************************************************
 * 数据导出回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::exportData(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    bool isVisible = rescueDataExportWidget.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - menuManager.width()) / 2;
    int y = r.y() + (r.height() - menuManager.height());
    rescueDataExportWidget.autoShow(x, y);
}

/***************************************************************************************************
 * 进入营救数据回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::clearData(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.setContent(SOFTKEY_ACTION_DEL_RESCUE_DATA);
}

/***************************************************************************************************
 * 退出营救数据回调。
 **************************************************************************************************/
void RescueDataSoftKeyAction::exit(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    SoftKeyActionType type = softkeyManager.uFaceTypeToSoftKeyType(windowManager.getUFaceType());
    softkeyManager.setContent(type, true);
}

/***************************************************************************************************
 * 获取动作描述总个数。
 **************************************************************************************************/
int RescueDataSoftKeyAction::getActionDescNR(void)
{
    return sizeof(_rescueDataKeys) / sizeof(KeyActionDesc);
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *RescueDataSoftKeyAction::getActionDesc(int index)
{
    if (index >= (int)(sizeof(_rescueDataKeys) / sizeof(KeyActionDesc)))
    {
        return NULL;
    }

    if (_rescueDataKeys[index].type < SOFT_BASE_KEY_NR)
    {
        return SoftkeyActionBase::getActionDesc(_rescueDataKeys[index].type);
    }

    return &_rescueDataKeys[index];
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
RescueDataSoftKeyAction::RescueDataSoftKeyAction() : SoftkeyActionBase(SOFTKEY_ACTION_RESCUE_DATA)
{

}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
RescueDataSoftKeyAction::~RescueDataSoftKeyAction()
{

}