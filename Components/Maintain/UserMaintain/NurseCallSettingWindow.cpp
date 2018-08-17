/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/16
 **/
#include "NurseCallSettingWindow.h"
#include <QLabel>
#include <QGridLayout>
#include "Button.h"
#include "ComboBox.h"
#include "IConfig.h"
#include "ThemeManager.h"
#include <QMap>

class NurseCallSettingWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_HIGH = 0,
        ITEM_BTN_MED,
        ITEM_BTN_LOW,
        ITEM_BTN_PHYS,
        ITEM_BTN_TECH,
    };

    NurseCallSettingWindowPrivate()
        : signalTypeCombo(NULL),
          triggerModeCombo(NULL),
          highBtn(NULL),
          medBtn(NULL),
          lowBtn(NULL),
          physBtn(NULL),
          techBtn(NULL)
    {
        btnMaps.clear();
    }

    ComboBox *signalTypeCombo;
    ComboBox *triggerModeCombo;

    Button *highBtn;
    Button *medBtn;
    Button *lowBtn;
    Button *physBtn;
    Button *techBtn;

    QMap<int, bool> btnMaps;
};

NurseCallSettingWindow::NurseCallSettingWindow()
    : d_ptr(new NurseCallSettingWindowPrivate)

{
    layoutExec();
}

NurseCallSettingWindow::~NurseCallSettingWindow()
{
    delete d_ptr;
}

void NurseCallSettingWindow::layoutExec()
{

    setWindowTitle(trs("NurseCallSettingWindow"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    Button *button;
    ComboBox *combo;
    int itemID;
    int index = 0;
    int tmpIndex;

    label = new QLabel(trs("SignalType"));
    layout->addWidget(label, index, 0);
    combo = new ComboBox;
    d_ptr->signalTypeCombo = combo;
    combo->addItems(QStringList()
                    << trs("Continuity")
                    << trs("Pluse")
                   );
    layout->addWidget(combo, index, 2);
    itemID = 0;
    combo->setProperty("Item",
                       qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    index++;
    systemConfig.getNumValue("Others|SignalType", tmpIndex);
    if (tmpIndex >= combo->count())
    {
        combo->setCurrentIndex(0);
    }
    else
    {
        combo->setCurrentIndex(tmpIndex);
    }
    tmpIndex = 0;

    label = new QLabel(trs("TriggerMode"));
    layout->addWidget(label, index, 0);
    combo = new ComboBox;
    d_ptr->triggerModeCombo = combo;
    combo->addItems(QStringList()
                    << trs("AllClosed")
                    << trs("AllOpened")
                   );
    layout->addWidget(combo, index, 2);
    itemID = 1;
    combo->setProperty("Item",
                       qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    index++;
    systemConfig.getNumValue("Others|SignalType", tmpIndex);
    if (tmpIndex >= combo->count())
    {
        combo->setCurrentIndex(0);
    }
    else
    {
        combo->setCurrentIndex(tmpIndex);
    }
    tmpIndex = 0;

    label = new QLabel(trs("AlarmLevel"));
    layout->addWidget(label, index, 0);
    label = new QLabel(trs("AlarmType"));
    layout->addWidget(label, index, 2);
    index++;

    int indexBtn = index;
    bool isSelected;
    QIcon icon = themeManger.getIcon(ThemeManager::IconChecked);
    button = new Button(trs("High"));
    button->setIcon(icon);
    button->setMinimumWidth(150);
    d_ptr->highBtn = button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = 0;
    button->setProperty("Item",
                        qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    layout->addWidget(button, indexBtn, 0);
    indexBtn++;
    systemConfig.getNumValue("Others|AlarmLevel|High", isSelected);

    if (isSelected)
    {
        button->setButtonStyle(Button::ButtonTextUnderIcon);
    }
    d_ptr->btnMaps[itemID] = isSelected;
    isSelected = false;

    button = new Button(trs("Med"));
    button->setIcon(icon);
    d_ptr->medBtn = button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = 1;
    button->setProperty("Item",
                        qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    layout->addWidget(button, indexBtn, 0);
    indexBtn++;
    systemConfig.getNumValue("Others|AlarmLevel|Med", isSelected);
    if (isSelected)
    {
        button->setButtonStyle(Button::ButtonTextUnderIcon);
    }
    d_ptr->btnMaps[itemID] = isSelected;
    isSelected = false;

    button = new Button(trs("Low"));
    button->setIcon(icon);
    d_ptr->lowBtn = button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = 2;
    button->setProperty("Item",
                        qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    layout->addWidget(button, indexBtn, 0);
    indexBtn++;
    systemConfig.getNumValue("Others|AlarmLevel|Low", isSelected);
    if (isSelected)
    {
        button->setButtonStyle(Button::ButtonTextUnderIcon);
    }
    d_ptr->btnMaps[itemID] = isSelected;
    isSelected = false;

    button = new Button(trs("Technology"));
    button->setIcon(icon);
    d_ptr->techBtn = button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = 3;
    button->setProperty("Item",
                        qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    layout->addWidget(button, index, 2);
    index++;
    systemConfig.getNumValue("Others|AlarmType|Technology", isSelected);
    if (isSelected)
    {
        button->setButtonStyle(Button::ButtonTextUnderIcon);
    }
    d_ptr->btnMaps[itemID] = isSelected;
    isSelected = false;

    button = new Button(trs("Physiology"));
    button->setIcon(icon);
    button->setMinimumWidth(150);
    d_ptr->physBtn = button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = 4;
    button->setProperty("Item",
                        qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    layout->addWidget(button, index, 2);
    index++;
    systemConfig.getNumValue("Others|AlarmType|Physiology", isSelected);
    if (isSelected)
    {
        button->setButtonStyle(Button::ButtonTextUnderIcon);
    }
    d_ptr->btnMaps[itemID] = isSelected;
    isSelected = false;

    layout->setRowStretch(indexBtn, 1);
    layout->setColumnMinimumWidth(1, 200);
    setWindowLayout(layout);
}

void NurseCallSettingWindow::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString strPath;
    if (indexType == 0)
    {
        strPath = "Others|SignalType";
    }
    else if (indexType == 1)
    {
        strPath = "Others|TriggerMode";
    }
    systemConfig.setNumValue(strPath, index);
}

void NurseCallSettingWindow::onBtnReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    int indexType = button->property("Item").toInt();
    QString strPath;

    switch (indexType)
    {
    case NurseCallSettingWindowPrivate::ITEM_BTN_HIGH:
        strPath = "Others|AlarmLevel|High";
        break;
    case NurseCallSettingWindowPrivate::ITEM_BTN_MED:
        strPath = "Others|AlarmLevel|Med";
        break;
    case NurseCallSettingWindowPrivate::ITEM_BTN_LOW:
        strPath = "Others|AlarmLevel|Low";
        break;
    case NurseCallSettingWindowPrivate::ITEM_BTN_PHYS:
        strPath = "Others|AlarmType|Technology";
        break;
    case NurseCallSettingWindowPrivate::ITEM_BTN_TECH:
        strPath = "Others|AlarmType|Physiology";
        break;
    default:
        break;
    }
    d_ptr->btnMaps[indexType] = !d_ptr->btnMaps[indexType];

    if (d_ptr->btnMaps[indexType])
    {
        button->setButtonStyle(Button::ButtonTextUnderIcon);
    }
    else
    {
        button->setButtonStyle(Button::ButtonTextOnly);
    }

    systemConfig.setNumValue(strPath, d_ptr->btnMaps[indexType]);
}






