/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#include "UserMaintainGeneralMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "NumberInput.h"
#include "IMessageBox.h"
#include "Button.h"
#include "KeyBoardPanel.h"

class UserMaintainGeneralMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_MONITOR_NAME,
        ITEM_BTN_DEPARTMENT,
        ITEM_BTN_BED_NUMBER,
        ITEM_BTN_MODIFY_PASSWORD,
        ITEM_CBO_CHANGE_BEDNUMBER_RIGHT,
        ITEM_CBO_LANGUAGE
    };

    UserMaintainGeneralMenuContentPrivate() {}

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
};

void UserMaintainGeneralMenuContentPrivate::loadOptions()
{
    QString str;
    systemConfig.getStrValue("General|MonitorName", str);
    buttons[ITEM_BTN_MONITOR_NAME]->setText(str);

    systemConfig.getStrValue("General|Department", str);
    buttons[ITEM_BTN_DEPARTMENT]->setText(str);

    systemConfig.getStrValue("General|BedNumber", str);
    buttons[ITEM_BTN_BED_NUMBER]->setText(str);

    systemConfig.getStrValue("General|UserMaintainPassword", str);
    buttons[ITEM_BTN_MODIFY_PASSWORD]->setText(str);

    int index = 0;
    systemConfig.getNumValue("General|ChangeBedNumberRight", index);
    combos[ITEM_CBO_CHANGE_BEDNUMBER_RIGHT]->setCurrentIndex(index);

    currentConfig.getNumAttr("Local|Language" , "CurrentOption", index);
    combos[ITEM_CBO_LANGUAGE]->setCurrentIndex(index);
}

UserMaintainGeneralMenuContent::UserMaintainGeneralMenuContent()
    : MenuContent(trs("UserMaintainGeneralMenu"), trs("UserMaintainGeneralMenuDesc")),
      d_ptr(new UserMaintainGeneralMenuContentPrivate)
{
}

UserMaintainGeneralMenuContent::~UserMaintainGeneralMenuContent()
{
    delete d_ptr;
}

void UserMaintainGeneralMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void UserMaintainGeneralMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    Button *button;
    int itemID;

    // monitor name
    label = new QLabel(trs("MonitorName"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MONITOR_NAME);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MONITOR_NAME, button);

    // department
    label = new QLabel(trs("Department"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_DEPARTMENT);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_DEPARTMENT, button);

    // bed Number
    label = new QLabel(trs("BedNumber"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_BED_NUMBER);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_BED_NUMBER, button);

    // modify password
    label = new QLabel(trs("ModifyPassword"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MODIFY_PASSWORD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MODIFY_PASSWORD, button);

    // change bed number right
    label = new QLabel(trs("ChangeBedNumberRight"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Protected")
                       << trs("Open")
                      );
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_CBO_CHANGE_BEDNUMBER_RIGHT);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->combos.insert(UserMaintainGeneralMenuContentPrivate::ITEM_CBO_CHANGE_BEDNUMBER_RIGHT, comboBox);

    // language setup
    label = new QLabel(trs("Language"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("English")
                       << trs("Chinese")
                      );
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_CBO_LANGUAGE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->combos.insert(UserMaintainGeneralMenuContentPrivate::ITEM_CBO_LANGUAGE, comboBox);

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count(), 1);
}

void UserMaintainGeneralMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        UserMaintainGeneralMenuContentPrivate::MenuItem item
                = (UserMaintainGeneralMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case UserMaintainGeneralMenuContentPrivate::ITEM_CBO_CHANGE_BEDNUMBER_RIGHT:
            systemConfig.setNumValue("General|ChangeBedNumberRight", index);
            break;
        case UserMaintainGeneralMenuContentPrivate::ITEM_CBO_LANGUAGE:
        {
            Config *config = configManager.getConfig(PATIENT_TYPE_ADULT);
            config->setNumAttr("Local|Language" , "CurrentOption" , index);
            config->saveToDisk();
            config = configManager.getConfig(PATIENT_TYPE_PED);
            config->setNumAttr("Local|Language" , "CurrentOption" , index);
            config->saveToDisk();
            config = configManager.getConfig(PATIENT_TYPE_NEO);
            config->setNumAttr("Local|Language" , "CurrentOption" , index);
            config->saveToDisk();
            break;
        }
        default:
            break;
        }
    }
}

void UserMaintainGeneralMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        UserMaintainGeneralMenuContentPrivate::MenuItem item
                = (UserMaintainGeneralMenuContentPrivate::MenuItem)button->property("Item").toInt();
        switch (item)
        {
        case UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MONITOR_NAME:
        {
            KeyBoardPanel idPanel;
            idPanel.setTitleBarText(trs("EditMonitorName"));
            idPanel.setInitString(button->text());
            idPanel.setMaxInputLength(MAX_DEVICE_ID_LEN - 1);

            QString regKeyStr("[a-zA-Z]|[0-9]|_");
            idPanel.setBtnEnable(regKeyStr);

            if (1 == idPanel.exec())
            {
                QString oldStr = button->text();
                QString text = idPanel.getStrValue();

                if (oldStr != text)
                {
                    button->setText(text);
                    systemConfig.setStrValue("General|MonitorName", text);
                }
            }
            break;
        }
        case UserMaintainGeneralMenuContentPrivate::ITEM_BTN_DEPARTMENT:
        {
            KeyBoardPanel idPanel;
            idPanel.setTitleBarText(trs("Department"));
            idPanel.setInitString(button->text());
            idPanel.setMaxInputLength(11);
            QString regKeyStr("[a-zA-Z]|[0-9]|_");
            idPanel.setBtnEnable(regKeyStr);

            if (1 == idPanel.exec())
            {
                QString oldStr = button->text();
                QString text = idPanel.getStrValue();

                if (oldStr != text)
                {
                    button->setText(text);
                    systemConfig.setStrValue("General|Department", text);
                }
            }
            break;
        }
        case UserMaintainGeneralMenuContentPrivate::ITEM_BTN_BED_NUMBER:
        {
            KeyBoardPanel idPanel;
            idPanel.setTitleBarText(trs("BedNumber"));
            idPanel.setInitString(button->text());
            idPanel.setMaxInputLength(11);
            QString regKeyStr("[a-zA-Z]|[0-9]|_");
            idPanel.setBtnEnable(regKeyStr);

            if (1 == idPanel.exec())
            {
                QString oldStr = button->text();
                QString text = idPanel.getStrValue();

                if (oldStr != text)
                {
                    button->setText(text);
                    systemConfig.setStrValue("General|BedNumber", text);
                }
            }
            break;
        }
        case UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MODIFY_PASSWORD:
        {
            KeyBoardPanel idPanel(KeyBoardPanel::KEY_TYPE_NUMBER);
            idPanel.setTitleBarText(trs("ModifyPassword"));
            idPanel.setInitString(button->text());
            idPanel.setMaxInputLength(11);
            QString regKeyStr("[a-zA-Z]|[0-9]|_");
            idPanel.setBtnEnable(regKeyStr);

            if (1 == idPanel.exec())
            {
                QString oldStr = button->text();
                QString text = idPanel.getStrValue();

                if (oldStr != text)
                {
                    button->setText(text);
                    systemConfig.setStrValue("General|UserMaintainPassword", text);
                }
            }
            break;
        }
        default:
            break;
        }
    }
}