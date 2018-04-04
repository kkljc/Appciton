#include <QVBoxLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "SupervisorAlarmMenu.h"
#include "SupervisorConfigManager.h"
#include "SupervisorMenuManager.h"
#include "AlarmSymbol.h"
#include "SoundManager.h"
#include "AlarmStateDefine.h"

SupervisorAlarmMenu *SupervisorAlarmMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorAlarmMenu::SupervisorAlarmMenu() : SubMenu(trs("AlarmSettingMenu"))
{
    setDesc(trs("AlarmSettingMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SupervisorAlarmMenu::readyShow()
{
    int index = 0;
//    superConfig.getNumValue("Alarm|LTAAlarm", index);
//    _ltaAlarm->setCurrentIndex(index);

    superConfig.getNumValue("Alarm|MinimumAlarmVolume", index);
    _minAlarmVolume->setCurrentIndex(index);

    _defaultAlarmVolume->clear();
    for (int i = index + 1; i <= SOUND_VOL_5; ++i)
    {
        _defaultAlarmVolume->addItem(trs(QString::number(i)));
    }
    QString str;
    superConfig.getStrValue("Alarm|DefaultAlarmVolume", str);
    _defaultAlarmVolume->setCurrentItem(str);

    superConfig.getNumValue("Alarm|AlarmPauseTime", index);
    _alarmPauseTime->setCurrentIndex(index);

    superConfig.getNumValue("Alarm|AlarmOffPrompting", index);
    _alarmClosePromptTime->setCurrentIndex(index);

    superConfig.getNumValue("Alarm|EnableAlarmAudioOff", index);
    _enableAlarmAudioOff->setCurrentIndex(index);

    superConfig.getNumValue("Alarm|EnableAlarmOff", index);
    _enableAlarmOff->setCurrentIndex(index);

    if (0 == index)
    {
        _alarmOffAtPowerOn->setDisabled(true);
    }
    else
    {
        _alarmOffAtPowerOn->setDisabled(false);
    }
    superConfig.getNumValue("Alarm|AlarmOffAtPowerOn", index);
    _alarmOffAtPowerOn->setCurrentIndex(index);

    superConfig.getNumValue("Alarm|NonAlertsBeepsInNonAED", index);
    _nonAlertsBeepsInNonAED->setCurrentIndex(index);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorAlarmMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //生命报警开关
//    _ltaAlarm = new IComboList(trs("LTAAlarm"));
//    _ltaAlarm->setFont(fontManager.textFont(fontsize));
//    _ltaAlarm->label->setFixedSize(labelWidth, ITEM_H);
//    _ltaAlarm->combolist->setFixedSize(btnWidth, ITEM_H);
//    _ltaAlarm->addItem(trs("Off"));
//    _ltaAlarm->addItem(trs("On"));
//    connect(_ltaAlarm, SIGNAL(currentIndexChanged(int)), this,
//            SLOT(_ltaAlarmChangeSlot(int)));
//    mainLayout->addWidget(_ltaAlarm);

    //最小报警音量
    _minAlarmVolume = new IComboList(trs("MinimumALarmVolume"));
    _minAlarmVolume->setFont(fontManager.textFont(fontSize));
    _minAlarmVolume->label->setFixedSize(labelWidth, ITEM_H);
    _minAlarmVolume->combolist->setFixedSize(btnWidth, ITEM_H);

    for(unsigned i = SOUND_VOL_1; i <= SOUND_VOL_5; ++i)
    {
        _minAlarmVolume->addItem(QString::number(i));
    }
    connect(_minAlarmVolume, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_minAlarmVolumeChangeSlot(int)));
    mainLayout->addWidget(_minAlarmVolume);

    //默认报警音量
    _defaultAlarmVolume = new IComboList(trs("DefaultAlarmVolume"));
    _defaultAlarmVolume->setFont(fontManager.textFont(fontSize));
    _defaultAlarmVolume->label->setFixedSize(labelWidth, ITEM_H);
    _defaultAlarmVolume->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_defaultAlarmVolume, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_defaultAlarmVolumeChangeSlot(int)));
    mainLayout->addWidget(_defaultAlarmVolume);

    //报警暂停时间
    _alarmPauseTime = new IComboList(trs("AlarmPauseTime"));
    _alarmPauseTime->setFont(fontManager.textFont(fontSize));
    _alarmPauseTime->label->setFixedSize(labelWidth, ITEM_H);
    _alarmPauseTime->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < ALARM_PAUSE_TIME_NR; ++i)
    {
        _alarmPauseTime->addItem(trs(AlarmSymbol::convert((AlarmPauseTime)i)));
    }

    connect(_alarmPauseTime, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_alarmPauseTimeChangeSlot(int)));
    mainLayout->addWidget(_alarmPauseTime);

    //报警关闭，报警音关闭提示时间
    _alarmClosePromptTime = new IComboList(trs("AlarmOffPromptMechanism"));
    _alarmClosePromptTime->setFont(fontManager.textFont(fontSize));
    _alarmClosePromptTime->label->setFixedSize(labelWidth, ITEM_H);
    _alarmClosePromptTime->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < ALARM_CLOSE_PROMPT_NR; ++i)
    {
        _alarmClosePromptTime->addItem(trs(AlarmSymbol::convert((AlarmClosePromptTime)i)));
    }

    connect(_alarmClosePromptTime, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_alarmClosePromptTimeChangeSlot(int)));
    mainLayout->addWidget(_alarmClosePromptTime);

    //eable alarm audio off
    _enableAlarmAudioOff = new IComboList(trs("EnableAlarmAudioOff"));
    _enableAlarmAudioOff->setFont(fontManager.textFont(fontSize));
    _enableAlarmAudioOff->label->setFixedSize(labelWidth, ITEM_H);
    _enableAlarmAudioOff->combolist->setFixedSize(btnWidth, ITEM_H);

    _enableAlarmAudioOff->addItem(trs("No"));
    _enableAlarmAudioOff->addItem(trs("Yes"));

    connect(_enableAlarmAudioOff, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_enableAlarmAudioOffChangeSlot(int)));
    mainLayout->addWidget(_enableAlarmAudioOff);

    //enable alarm off
    _enableAlarmOff = new IComboList(trs("EnableAlarmOff"));
    _enableAlarmOff->setFont(fontManager.textFont(fontSize));
    _enableAlarmOff->label->setFixedSize(labelWidth, ITEM_H);
    _enableAlarmOff->combolist->setFixedSize(btnWidth, ITEM_H);

    _enableAlarmOff->addItem(trs("No"));
    _enableAlarmOff->addItem(trs("Yes"));

    connect(_enableAlarmOff, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_enableAlarmOffChangeSlot(int)));
    mainLayout->addWidget(_enableAlarmOff);

    //alarm off at power on
    _alarmOffAtPowerOn = new IComboList(trs("AlarmOffAtPowerOn"));
    _alarmOffAtPowerOn->setFont(fontManager.textFont(fontSize));
    _alarmOffAtPowerOn->label->setFixedSize(labelWidth, ITEM_H);
    _alarmOffAtPowerOn->combolist->setFixedSize(btnWidth, ITEM_H);

    _alarmOffAtPowerOn->addItem(trs("No"));
    _alarmOffAtPowerOn->addItem(trs("Yes"));

    connect(_alarmOffAtPowerOn, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_alarmOffAtPowerOnChangeSlot(int)));
    mainLayout->addWidget(_alarmOffAtPowerOn);

    _nonAlertsBeepsInNonAED = new IComboList(trs("NonAlertsBeepsInNonAED"));
    _nonAlertsBeepsInNonAED->setFont(fontManager.textFont(fontSize));
    _nonAlertsBeepsInNonAED->label->setFixedSize(labelWidth, ITEM_H);
    _nonAlertsBeepsInNonAED->combolist->setFixedSize(btnWidth, ITEM_H);
    _nonAlertsBeepsInNonAED->addItem(trs("Disable"));
    _nonAlertsBeepsInNonAED->addItem(trs("Enable"));
    connect(_nonAlertsBeepsInNonAED, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_nonAlertsBeepsInNonAEDSlot(int)));
    mainLayout->addWidget(_nonAlertsBeepsInNonAED);
}

/**************************************************************************************************
 * 生命报警开关。
 *************************************************************************************************/
//void SupervisorAlarmMenu::_ltaAlarmChangeSlot(int index)
//{
//    superConfig.setNumValue("Alarm|LTAAlarm", index);
//}

/**************************************************************************************************
 * 最低报警音量。
 *************************************************************************************************/
void SupervisorAlarmMenu::_minAlarmVolumeChangeSlot(int index)
{
    superConfig.setNumValue("Alarm|MinimumAlarmVolume", index);
    _defaultAlarmVolume->clear();
    for (int i = index + 1; i <= SOUND_VOL_5; ++i)
    {
        _defaultAlarmVolume->addItem(trs(QString::number(i)));
    }
    QString str;
    superConfig.getStrValue("Alarm|DefaultAlarmVolume", str);
    if (str.toInt() < (index + 1))
    {
        superConfig.setNumValue("Alarm|DefaultAlarmVolume", index + 1);
        _defaultAlarmVolume->setCurrentIndex(0);
    }
    else
    {
        _defaultAlarmVolume->setCurrentItem(str);
    }
}

/**************************************************************************************************
 * 默认报警音量。
 *************************************************************************************************/
void SupervisorAlarmMenu::_defaultAlarmVolumeChangeSlot(int index)
{
    QString str = _defaultAlarmVolume->itemText(index);
    superConfig.setStrValue("Alarm|DefaultAlarmVolume", str);
}

/**************************************************************************************************
 * 报警暂停时间。
 *************************************************************************************************/
void SupervisorAlarmMenu::_alarmPauseTimeChangeSlot(int index)
{
    superConfig.setNumValue("Alarm|AlarmPauseTime", index);
}

/**************************************************************************************************
 * 报警关闭提示机制。
 *************************************************************************************************/
void SupervisorAlarmMenu::_alarmClosePromptTimeChangeSlot(int index)
{
    superConfig.setNumValue("Alarm|AlarmOffPrompting", index);
}

/**************************************************************************************************
 * enable alarm audio off。
 *************************************************************************************************/
void SupervisorAlarmMenu::_enableAlarmAudioOffChangeSlot(int index)
{
    superConfig.setNumValue("Alarm|EnableAlarmAudioOff", index);
}

/**************************************************************************************************
 * enable alarm off。
 *************************************************************************************************/
void SupervisorAlarmMenu::_enableAlarmOffChangeSlot(int index)
{
    superConfig.setNumValue("Alarm|EnableAlarmOff", index);

    if (0 == index)
    {
        _alarmOffAtPowerOn->setDisabled(true);
    }
    else
    {
        _alarmOffAtPowerOn->setDisabled(false);
    }
}

/**************************************************************************************************
 * alarm off at power on。
 *************************************************************************************************/
void SupervisorAlarmMenu::_alarmOffAtPowerOnChangeSlot(int index)
{
    superConfig.setNumValue("Alarm|AlarmOffAtPowerOn", index);
}

/**************************************************************************************************
 * Non-alerts beeps in Non-AED modes。
 *************************************************************************************************/
void SupervisorAlarmMenu::_nonAlertsBeepsInNonAEDSlot(int index)
{
    superConfig.setNumValue("Alarm|NonAlertsBeepsInNonAED", index);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorAlarmMenu::~SupervisorAlarmMenu()
{

}

