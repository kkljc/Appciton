/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/7
 **/


#include "IBPTrendWidget.h"
#include "IBPParam.h"
#include "PublicMenuManager.h"
#include "WindowManager.h"
#include "TrendWidgetLabel.h"
#include "ParamManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "IBPMenu.h"
#include <QDebug>
#include <QGroupBox>
#include "MeasureSettingWindow.h"

/**************************************************************************************************
 * 设置测量结果的数据。
 *************************************************************************************************/
void IBPTrendWidget::setData(int16_t sys, int16_t dia, int16_t map)
{
    if ((_entitle < IBP_PRESSURE_CVP) || (_entitle > IBP_PRESSURE_ICP))
    {
        if (!_isZero)
        {
            setShowStacked(0);
        }
        else if ((sys == InvData()) || (dia == InvData()) || (map == InvData()))
        {
            setShowStacked(1);
            _sysString = InvStr();
            _diaString = InvStr();
            _mapString = InvStr();
        }
        else
        {
            int16_t calSys = sys;
            int16_t calDia = dia;
            int16_t calMap = map;
            setShowStacked(1);
            _sysString = QString::number(calSys);
            _diaString = QString::number(calDia);
            _mapString = "(" + QString::number(calMap) + ")";
        }
    }
    else
    {
        if (!_isZero)
        {
            setShowStacked(0);
        }
        else if (map == InvData())
        {
            setShowStacked(2);
            _veinString = InvStr();
        }
        else
        {
            int16_t calMap = map;
            setShowStacked(2);
            _veinString = QString::number(calMap);
        }
    }

    _sysValue->setText(_sysString);
    _diaValue->setText(_diaString);
    _mapValue->setText(_mapString);
    _veinValue->setText(_veinString);

    return;
}

/**************************************************************************************************
 * 设置标名。
 *************************************************************************************************/
void IBPTrendWidget::setEntitle(IBPPressureName entitle)
{
    setName(IBPSymbol::convert(entitle));
    _entitle = entitle;
    if (entitle >= IBP_PRESSURE_CVP && entitle <= IBP_PRESSURE_ICP)
    {
        setShowStacked(2);
    }
    else
    {
        setShowStacked(1);
    }
}

/**************************************************************************************************
 * IBP显示栏设置。
 * 0、提示校零
 * 1、动脉压数据信息
 * 2、静脉压数据信息
*************************************************************************************************/
void IBPTrendWidget::setShowStacked(int num)
{
    _stackedwidget->setCurrentIndex(num);
}

/**************************************************************************************************
 * 设置校零标志。
 *************************************************************************************************/
void IBPTrendWidget::setZeroFlag(bool isEnabled)
{
    _isZero = isEnabled;
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void IBPTrendWidget::isAlarm(int id, bool flag)
{
    SubParamID subID = (SubParamID)id;
    switch (subID)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
        _sysAlarm = flag;
        break;
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP2_DIA:
        _diaAlarm = flag;
        break;
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
        _mapAlarm = flag;
        break;
    default:
        break;
    }

    updateAlarm(_sysAlarm || _diaAlarm || _mapAlarm);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void IBPTrendWidget::showValue()
{
    QPalette p = palette();
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    psrc = normalPalette(psrc);
    if (_sysAlarm || _diaAlarm || _mapAlarm)
    {
        if (_sysAlarm)
        {
            showAlarmStatus(_sysValue, psrc);
        }
        else
        {
            showNormalStatus(_sysValue, psrc);
        }

        if (_diaAlarm)
        {
            showAlarmStatus(_diaValue, psrc);
        }
        else
        {
            showNormalStatus(_diaValue, psrc);
        }

        if (_mapAlarm)
        {
            showAlarmStatus(_mapValue, psrc);
            showAlarmStatus(_veinValue, psrc);
        }
        else
        {
            showNormalStatus(_mapValue, psrc);
            showNormalStatus(_veinValue, psrc);
        }
    }
    else
    {
        setPalette(psrc);
        showNormalStatus(_sysValue, psrc);
        showNormalStatus(_diaValue, psrc);
        showNormalStatus(_mapValue, psrc);
        showNormalStatus(_veinValue, psrc);
        showNormalStatus(_ibpValue, psrc);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPTrendWidget::IBPTrendWidget(const QString &trendName, const IBPPressureName &entitle) : TrendWidget(trendName),
    _isZero(false), _sysAlarm(false), _diaAlarm(false), _mapAlarm(false)
{
    _sysString = InvStr();
    _diaString = InvStr();
    _mapString = InvStr();
    _veinString = InvStr();

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    setPalette(palette);
    setName(IBPSymbol::convert(entitle));
    setUnit("mmHg");

    // 构造出所有控件。

    _zeroWarn = new QLabel();
    _zeroWarn->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _zeroWarn->setPalette(palette);
    _zeroWarn->setText(trs("ZeroRequired"));

    _ibpValue = new QLabel();
    _ibpValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _ibpValue->setPalette(palette);
    _ibpValue->setText("/");

    _sysValue = new QLabel();
    _sysValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _sysValue->setPalette(palette);
    _sysValue->setText(InvStr());

    _diaValue = new QLabel();
    _diaValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _diaValue->setPalette(palette);
    _diaValue->setText(InvStr());

    _mapValue = new QLabel();
    _mapValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _mapValue->setPalette(palette);
    _mapValue->setText(InvStr());

    _veinValue = new QLabel();
    _veinValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _veinValue->setPalette(palette);
    _veinValue->setText(InvStr());

    // 开始布局
    QGroupBox *_groupBox0 = new QGroupBox();
    _groupBox0->setStyleSheet("border:none");
    QHBoxLayout *hLayout0 = new QHBoxLayout();
    hLayout0->setMargin(_margin);
    hLayout0->addStretch();
    hLayout0->addWidget(_zeroWarn);
    hLayout0->addStretch();
    hLayout0->setAlignment(Qt::AlignVCenter);
    _groupBox0->setLayout(hLayout0);

    QGroupBox *_groupBox1 = new QGroupBox();
    _groupBox1->setStyleSheet("border:none");
    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->setMargin(_margin);
    hLayout1->setSpacing(10);
    hLayout1->addStretch();
    hLayout1->addWidget(_sysValue);
    hLayout1->addWidget(_ibpValue);
    hLayout1->addWidget(_diaValue);
    hLayout1->addWidget(_mapValue);
    hLayout1->addStretch();
    hLayout1->setAlignment(Qt::AlignVCenter);
    _groupBox1->setLayout(hLayout1);

    QGroupBox *_groupBox2 = new QGroupBox();
    _groupBox2->setStyleSheet("border:none");
    QHBoxLayout *hLayout2 = new QHBoxLayout();
    hLayout2->setMargin(_margin);
    hLayout2->addStretch();
    hLayout2->addWidget(_veinValue);
    hLayout2->addStretch();
    hLayout2->setAlignment(Qt::AlignVCenter);
    _groupBox2->setLayout(hLayout2);

    _stackedwidget = new QStackedWidget();
    _stackedwidget->addWidget(_groupBox0);
    _stackedwidget->addWidget(_groupBox1);
    _stackedwidget->addWidget(_groupBox2);

    contentLayout->addWidget(_stackedwidget, Qt::AlignCenter);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPTrendWidget::~IBPTrendWidget()
{
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void IBPTrendWidget::setTextSize()
{
    QRect r;
    r.setSize(QSize(((width() - nameLabel->width()) / 4), ((height() / 4) * 3)));
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _ibpValue->setFont(font);
    _sysValue->setFont(font);
    _diaValue->setFont(font);
    _mapValue->setFont(font);

    _veinValue->setFont(font);

    font = fontManager.numFont(fontsize - 10, true);
    font.setWeight(QFont::Black);
    _mapValue->setFont(font);

    font = fontManager.numFont(fontsize - 20, true);
    font.setWeight(QFont::Normal);
    _zeroWarn->setFont(font);
}

void IBPTrendWidget::_releaseHandle(IWidget *iWidget)
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("IBPMenu"));
}
