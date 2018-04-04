﻿#include <QApplication>
#include "SoftkeyActionBase.h"
#include "SoftKeyManager.h"
#include "SoftKeyWidget.h"
#include "ECGParam.h"
#include "ECGMenu.h"
#include "PublicMenuManager.h"
#include "AlarmLimitMenu.h"
#include "CodeMarkerWidget.h"
#include "SystemBoardProvider.h"
#include "WindowManager.h"
#include "WindowLayout.h"
#include "PatientInfoMenu.h"
#include "IMessageBox.h"
#include "CO2Param.h"
#include "SystemManager.h"
#include "DataStorageDirManager.h"
#include "DoseCalculationWidget.h"
#include "DoseCalculationManager.h"
#include "HemodynamicWidget.h"
#include "ECG12LDataAcquire.h"
#include "ECG12LeadManager.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _baseKeys[] =
{
//    KeyActionDesc("", "LeadSelection.png", SoftkeyActionBase::ecgLeadChange),
//    KeyActionDesc("", "CodeMarker.png",    SoftkeyActionBase::codeMarker),
//    KeyActionDesc("", "Bell.png",          SoftkeyActionBase::limitMenu),
//    KeyActionDesc("", "Data.png",          SoftkeyActionBase::rescueData),
//    KeyActionDesc("", "PatientInfo.png",   SoftkeyActionBase::patientInfo),
//    KeyActionDesc("", "CO2Disable.png",    SoftkeyActionBase::co2OnOff),
//    KeyActionDesc("", "SoftkeyArrow.png",  SoftkeyActionBase::nextPage),
    KeyActionDesc("", "PatientInfo.png",   SoftkeyActionBase::patientInfo, SOFT_BASE_KEY_NR, true, QColor(143, 31, 132)),
    KeyActionDesc("", "PatientNew.png",   SoftkeyActionBase::patientNew, SOFT_BASE_KEY_NR, true, QColor(143, 31, 132)),
    KeyActionDesc("", "previous-disable.png",  SoftkeyActionBase::previousPage),
    KeyActionDesc("", "LeadSelection.png", SoftkeyActionBase::ecgLeadChange),
    KeyActionDesc("", "Bell.png",          SoftkeyActionBase::limitMenu),
    KeyActionDesc("", "CodeMarker.png",    SoftkeyActionBase::codeMarker),
    KeyActionDesc("", "Data.png",          SoftkeyActionBase::rescueData),
    KeyActionDesc("", "interface.png",    SoftkeyActionBase::WindowLayout),
    KeyActionDesc("", "dosecalculation.png",    SoftkeyActionBase::calculation),
    KeyActionDesc("", "next-enable.png",  SoftkeyActionBase::nextPage),
    KeyActionDesc("", "MainSetup.png",  SoftkeyActionBase::mainsetup, SOFT_BASE_KEY_NR, true, QColor(255, 200, 0)),
};

/***************************************************************************************************
 * 导联改变回调。
 **************************************************************************************************/
void SoftkeyActionBase::ecgLeadChange(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

//co2Param.setConnected(false);
//return;
    ecgParam.autoSetCalcLead();
}

/***************************************************************************************************
 * CodeMarker回调。
 **************************************************************************************************/
void SoftkeyActionBase::codeMarker(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

//co2Param.setConnected(true);
//return;

    bool isVisible = codeMarkerWidget.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

    codeMarkerWidget.autoShow();
}

void SoftkeyActionBase::previousPage(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.previousPage();
}

/***************************************************************************************************
 * 下一页面回调。
 **************************************************************************************************/
void SoftkeyActionBase::nextPage(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.nextPage();
}

/***************************************************************************************************
 * 报警限设置回调。
 **************************************************************************************************/
void SoftkeyActionBase::limitMenu(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    bool isVisible = publicMenuManager.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
    int y = r.y() + (r.height() - publicMenuManager.height());
    publicMenuManager.popup(&alarmLimitMenu, x, y);
}

void SoftkeyActionBase::WindowLayout(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    if (ecg12LDataAcquire.isAcquiring() || ecg12LeadManager.isTransfer())
    {
        return;
    }

    bool isVisible = windowLayout.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - patientMenu.width()) / 2;
//    int y = r.y() + (r.height() - patientMenu.height());
//    patientMenu.autoShow(x, y);
    //    patientMenu.autoShow();
    windowLayout.autoShow();
}

/***************************************************************************************************
 * 数据查看回调。
 **************************************************************************************************/
void SoftkeyActionBase::rescueData(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.setContent(SOFTKEY_ACTION_RESCUE_DATA);
}

void SoftkeyActionBase::calculation(bool isPressed)
{
    if (isPressed)
    {
        return;
    }

    softkeyManager.setContent(SOFTKEY_ACTION_CALCULATE);
}

/***************************************************************************************************
 * 病人信息。
 **************************************************************************************************/
void SoftkeyActionBase::patientInfo(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    if (ecg12LDataAcquire.isAcquiring() || ecg12LeadManager.isTransfer())
    {
        return;
    }

    bool isVisible = patientInfoMenu.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - patientMenu.width()) / 2;
//    int y = r.y() + (r.height() - patientMenu.height());
//    patientMenu.autoShow(x, y);
    //    patientMenu.autoShow();
    patientInfoMenu.newPatientStatus(false);
    patientInfoMenu.widgetChange();
    patientInfoMenu.autoShow();
}

void SoftkeyActionBase::patientNew(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    if (ecg12LDataAcquire.isAcquiring() || ecg12LeadManager.isTransfer())
    {
        return;
    }

    bool isVisible = patientInfoMenu.isVisible();
    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (isVisible)
    {
        return;
    }

    //创建新病人
    //patientMenu.createPatient();

//    QRect r = windowManager.getMenuArea();
//    int x = r.x() + (r.width() - patientMainMenu.width()) / 2;
//    int y = r.y() + (r.height() - patientMainMenu.height());
    QStringList slist;
    slist << trs("EnglishYESChineseSURE") << trs("No");
    IMessageBox messageBox(trs("Warn"), trs("RemoveAndRecePatient"), slist);
    if (messageBox.exec() == 0)
    {
        dataStorageDirManager.createDir(true);
        patientInfoMenu.newPatientStatus(true);
        patientInfoMenu.widgetChange();
        patientInfoMenu.autoShow();
    }
}

void SoftkeyActionBase::mainsetup(bool isPressed)
{
    if (isPressed)
    {
        return;
    }
    windowManager.setUFaceType();
}

/***************************************************************************************************
 * 获取动作描述总个数。
 **************************************************************************************************/
int SoftkeyActionBase::getActionDescNR(void)
{
    return 0;
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *SoftkeyActionBase::getActionDesc(int index)
{
    if (index > SOFT_BASE_KEY_NR)
    {
        return NULL;
    }

    if (index == SOFT_BASE_KEY_PREVIOUS_PAGE)
    {
        if (softkeyManager.returnPage() == 0)
        {
            _baseKeys[index].iconPath = "previous-disable.png";
            _baseKeys[index].focus = false;
        }
        else
        {
            _baseKeys[index].iconPath = "previous-enable.png";
            _baseKeys[index].focus = true;
        }
    }
    else if (index == SOFT_BASE_KEY_NEXT_PAGE)
        {
        if (softkeyManager.returnPage() == 1)
        {
            _baseKeys[index].iconPath = "next-disable.png";
            _baseKeys[index].focus = false;
        }
        else
        {
            _baseKeys[index].iconPath = "next-enable.png";
            _baseKeys[index].focus = true;
        }
    }

    return &_baseKeys[index];
}

/***************************************************************************************************
 * 获取类型。
 **************************************************************************************************/
SoftKeyActionType SoftkeyActionBase::getType(void)
{
    return _type;
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
SoftkeyActionBase::SoftkeyActionBase(SoftKeyActionType t)
{
    _type = t;
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
SoftkeyActionBase::~SoftkeyActionBase()
{
}
