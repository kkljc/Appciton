/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/23
 **/

#include "FactorySystemInfoMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include <Button.h>
#include <QGridLayout>
#include "KeyInputPanel.h"
#include "IConfig.h"
#include "WindowManager.h"

class FactorySystemInfoMenuContentPrivate
{
public:
    FactorySystemInfoMenuContentPrivate();
    /**
     * @brief loadOptions
     */
    void loadOptions();

    Button *serialNumBtn;
    Button *machineTypeBtn;
};

FactorySystemInfoMenuContentPrivate::FactorySystemInfoMenuContentPrivate():
    serialNumBtn(NULL), machineTypeBtn(NULL)
{
}

FactorySystemInfoMenuContent::FactorySystemInfoMenuContent():
    MenuContent(trs("SystemInfoSetting"),
                trs("SystemInfoSettingDesc")),
    d_ptr(new FactorySystemInfoMenuContentPrivate)
{
}

FactorySystemInfoMenuContent::~FactorySystemInfoMenuContent()
{
    delete d_ptr;
}

void FactorySystemInfoMenuContentPrivate::loadOptions()
{
    if (!serialNumBtn)
    {
        return;
    }
    QString str;
    machineConfig.getStrValue("SerialNumber", str);
    serialNumBtn->setText(str);

    systemConfig.getStrValue("MonitorInfo|MachineType", str);
    machineTypeBtn->setText(str);
}

void FactorySystemInfoMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void FactorySystemInfoMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    this->setFocusPolicy(Qt::NoFocus);

    QLabel *label = new QLabel(trs("SerialNum"));
    layout->addWidget(label, 0, 0);
    Button *button = new Button("");
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 0, 1);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    d_ptr->serialNumBtn = button;

    label = new QLabel(trs("ProductModel"));
    layout->addWidget(label, 1, 0);
    button = new Button("");
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 1, 1);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    d_ptr->machineTypeBtn = button;

    layout->setAlignment(Qt::AlignTop);
}

void FactorySystemInfoMenuContent::onBtnReleasedChanged()
{
    Button *button = qobject_cast<Button *>(sender());

    if (button == d_ptr->serialNumBtn)
    {
        KeyInputPanel panel;
        panel.setMaxInputLength(11);
        panel.setInitString(button->text());
        panel.setWindowTitle(trs("SerialNum"));
        panel.setSpaceEnable(false);
        panel.setSymbolEnable(false);
        panel.setBtnEnable("[a-zA-Z0-9]");

        windowManager.showWindow(&panel, WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
        if (panel.result() == QDialog::Rejected)
        {
            return;
        }

        QString str = panel.getStrValue();

        if (str == button->text())
        {
            return;
        }
        if (str.isNull())
        {
            return;
        }

        button->setText(str);
        machineConfig.setStrValue("SerialNumber", str);
        machineConfig.save();
        machineConfig.saveToDisk();
    }
    else if (button == d_ptr->machineTypeBtn)
    {
        KeyInputPanel panel;
        panel.setMaxInputLength(10);
        panel.setInitString(button->text());
        panel.setWindowTitle(trs("ProductModel"));
        panel.setSpaceEnable(true);
        panel.setSymbolEnable(true);

        windowManager.showWindow(&panel, WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
        if (panel.result() == QDialog::Rejected)
        {
            return;
        }

        QString str = panel.getStrValue();

        if (str == button->text())
        {
            return;
        }
        if (str.isNull())
        {
            return;
        }

        button->setText(str);
        systemConfig.setStrValue("MonitorInfo|MachineType", str);
    }
}


















