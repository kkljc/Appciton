/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include "NIBPDataTrendWidget.h"
#include "ParamManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "Debug.h"
#include "NIBPAlarm.h"
#include "WindowManager.h"
#include "ITableWidget.h"
#include "TimeDate.h"
#include "Alarm.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"

#define COLUMN_COUNT    3

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void NIBPDataTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("NIBPMenu"));
}

void NIBPDataTrendWidget::_releaseHandle()
{
}

/**************************************************************************************************
 * 设置HR的值。
 *************************************************************************************************/
void NIBPDataTrendWidget::setHRValue(int16_t hr, bool isHR)
{
    if (isHR)
    {
        setName(trs(paramInfo.getSubParamName(SUB_PARAM_HR_PR)));
    }
    else
    {
        setName(trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR)));
    }

    if (hr == InvData())
    {
        _hrString = InvStr();
    }
    else
    {
        _hrString = QString::number(hr);
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void NIBPDataTrendWidget::isAlarm(bool isAlarm)
{
    _isAlarm = isAlarm;

    updateAlarm(isAlarm);
}

/**************************************************************************************************
 * 函数说明：收集趋势数据。
 * 函数说明：收集趋势数据。
 * 函数参数：
 *         data:保存趋势数据
 *************************************************************************************************/
void NIBPDataTrendWidget::collectNIBPTrendData(unsigned t)
{
    if (_nibpNrendCacheMap.contains(t))
    {
        return;
    }

    NIBPTrendCacheData data;
    QList<ParamID> paramIDList;
    paramManager.getParams(paramIDList);

    data.lastNibpMeasureTime = t;
    data.sysvalue = paramManager.getSubParamValue(PARAM_NIBP, SUB_PARAM_NIBP_SYS);
    data.diavalue = paramManager.getSubParamValue(PARAM_NIBP, SUB_PARAM_NIBP_DIA);
    data.mapvalue = paramManager.getSubParamValue(PARAM_NIBP, SUB_PARAM_NIBP_MAP);
    data.valueIsDisplay = paramManager.isParamEnable(PARAM_NIBP);
    data.prvalue = nibpParam.getPR();

    if ((data.sysvalue != InvData())
            && (data.sysvalue != InvData())
            && (data.sysvalue != InvData())
            && (data.prvalue != InvData())
            )
    {
        int completeResult = nibpLimitAlarm.getCompare(data.sysvalue, NIBP_LIMIT_ALARM_SYS_LOW);
        if (completeResult != 0)
        {
            data.sysAlarm = true;
        }
        completeResult = nibpLimitAlarm.getCompare(data.sysvalue, NIBP_LIMIT_ALARM_SYS_HIGH);
        if (completeResult != 0)
        {
            data.sysAlarm = true;
        }

        completeResult = nibpLimitAlarm.getCompare(data.diavalue, NIBP_LIMIT_ALARM_DIA_LOW);
        if (completeResult != 0)
        {
            data.diaAlarm = true;
        }
        completeResult = nibpLimitAlarm.getCompare(data.diavalue, NIBP_LIMIT_ALARM_DIA_HIGH);
        if (completeResult != 0)
        {
            data.diaAlarm = true;
        }

        completeResult = nibpLimitAlarm.getCompare(data.mapvalue, NIBP_LIMIT_ALARM_MAP_LOW);
        if (completeResult != 0)
        {
            data.mapAlarm = true;
        }
        completeResult = nibpLimitAlarm.getCompare(data.mapvalue, NIBP_LIMIT_ALARM_MAP_HIGH);
        if (completeResult != 0)
        {
            data.mapAlarm = true;
        }
    }

    if (10 <= _nibpNrendCacheMap.count())
    {
        _nibpNrendCacheMap.remove(_nibpNrendCacheMap.begin().key());
    }

    _nibpNrendCacheMap.insert(t, data);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void NIBPDataTrendWidget::showValue(void)
{
    NIBPTrendCacheMap::iterator t = _nibpNrendCacheMap.end() - 1;
    if (_nibpNrendCacheMap.end() == _nibpNrendCacheMap.begin())
    {
        return;
    }
    QString textStr;
    QString timeStr;
    QString prStr;
    UnitType defUnitType;
    UnitType unit;

    QColor textColor = colorManager.getColor(paramInfo.getParamName(PARAM_NIBP));

    for (int i = 0; i < _rowNR; i++)
    {
        _table->item(i, 0)->setText("");
        _table->item(i, 0)->setTextColor(textColor);
        QLabel *l = qobject_cast<QLabel *>(_table->cellWidget(i, 1));
        l->setText("");
        l = qobject_cast<QLabel *>(_table->cellWidget(i, 2));
        l->setText("");
    }

    for (int i = 0; i < _rowNR; i++)
    {
        timeDate.getTime(t.key(), timeStr);
        _table->item(i, 0)->setText(timeStr);

        NIBPTrendCacheData providerBuff = t.value();
        if (providerBuff.sysvalue == InvData() || providerBuff.diavalue == InvData() ||
                providerBuff.mapvalue == InvData())
        {
            textStr = QString("<center>%1/%2/%3</center>").arg(InvStr()).arg(InvStr()).arg(InvStr());
        }
        else
        {
            QString valStr;
            QString boldwrap = "<b>%1</b>";
            QString colorwrap = "<font color=red>%1</font>";
            QString color = "<font style='color:rgb(%1,%2,%3);'>%4</font>";
            textStr = QString("<center>%1/%2/%3</center>");
            prStr = QString("<center>%1</center>");

            defUnitType = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
            unit = paramManager.getSubParamUnit(PARAM_NIBP, SUB_PARAM_NIBP_SYS);
            if (unit == defUnitType)
            {
                if (providerBuff.sysAlarm)
                {
                    valStr = boldwrap.arg(colorwrap.arg(QString::number(providerBuff.sysvalue)));
                    textStr = textStr.arg(valStr);
                }
                else
                {
                    textStr = textStr.arg(QString::number(providerBuff.sysvalue));
                }

                if (providerBuff.diaAlarm)
                {
                    valStr = boldwrap.arg(colorwrap.arg(QString::number(providerBuff.diavalue)));
                    textStr = textStr.arg(valStr);
                }
                else
                {
                    textStr = textStr.arg(QString::number(providerBuff.diavalue));
                }

                if (providerBuff.mapAlarm)
                {
                    valStr = boldwrap.arg(colorwrap.arg(QString::number(providerBuff.mapvalue)));
                    textStr = textStr.arg(valStr);
                }
                else
                {
                    textStr = textStr.arg(QString::number(providerBuff.mapvalue));
                }
                if (!providerBuff.prAlarm)
                {
                    valStr = color.arg(textColor.red())
                            .arg(textColor.green())
                            .arg(textColor.blue())
                            .arg(QString::number(providerBuff.prvalue));
                    prStr = prStr.arg(valStr);
                }
                if (!providerBuff.sysAlarm && !providerBuff.diaAlarm && !providerBuff.mapAlarm)
                {
                    textStr = color.arg(textColor.red())
                            .arg(textColor.green())
                            .arg(textColor.blue())
                            .arg(textStr);
                }
            }
            else
            {
                textStr = textStr.arg(Unit::convert(unit, defUnitType, providerBuff.sysvalue))
                          .arg(Unit::convert(unit, defUnitType, providerBuff.diavalue))
                          .arg(Unit::convert(unit, defUnitType, providerBuff.mapvalue));
            }
        }
        QLabel *l = qobject_cast<QLabel *>(_table->cellWidget(i, 1));
        l->setAlignment(Qt::AlignHCenter);
        l->setText(textStr);
        l->setTextInteractionFlags(Qt::NoTextInteraction);

        QLabel *prLbl = qobject_cast<QLabel *>(_table->cellWidget(i, 2));
        prLbl->setAlignment(Qt::AlignHCenter);
        prLbl->setText(prStr);
        prLbl->setTextInteractionFlags(Qt::NoTextInteraction);

        if (t != _nibpNrendCacheMap.begin())
        {
            t = t - 1;
        }
        else
        {
            return;
        }
    }
}

/**************************************************************************************************
 * 窗体变化触发。
 *************************************************************************************************/
void NIBPDataTrendWidget::resizeEvent(QResizeEvent *e)
{
    TrendWidget::resizeEvent(e);
    _rowNR = _table->height() / _tableItemHeight - 1;
    _table->setRowCount(_rowNR);
    int eachColumnWidth = _table->width() / COLUMN_COUNT;
    _table->setColumnWidth(0, eachColumnWidth);
    _table->setColumnWidth(1, eachColumnWidth * 4/3);
    _table->setColumnWidth(2, eachColumnWidth * 2/3);

    for (int i = 0; i < _rowNR; i++)
    {
        QColor textColor = colorManager.getColor(paramInfo.getParamName(PARAM_NIBP));
        _table->setRowHeight(i, _tableItemHeight);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignCenter);
        item->setTextColor(textColor);
        _table->setItem(i, 0, item);

        QLabel *l = new QLabel();
        _table->setCellWidget(i, 1, l);
        l->setText("");

        QLabel *prLbl = new QLabel();
        _table->setCellWidget(i, 2, prLbl);
        prLbl->setText("");
    }
}

void NIBPDataTrendWidget::setTextSize()
{
    QRect r;
    r.setWidth((width() - nameLabel->width()) / 2.5);
    // 字体。
    int fontsize = fontManager.adjustNumFontSize(r, true, "220/150/100");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _table->setFont(font);

    _tableItemHeight = fontManager.textHeightInPixels(font);
}

void NIBPDataTrendWidget::updatePalette(const QPalette &pal)
{
    setPalette(pal);
    // 刷新背景
    QColor color = colorManager.getColor(paramInfo.getParamName(PARAM_NIBP));
    QString headStyle = QString("QHeaderView::section{color:rgb(%1,%2,%3);"
                                "border:0px solid black;"
                                "background-color:black;}")
                        .arg(color.red()).arg(color.green()).arg(color.blue());
    _table->horizontalHeader()->setStyleSheet(headStyle);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPDataTrendWidget::NIBPDataTrendWidget()
    : TrendWidget("NIBPDataTrendWidget"),
      _hrString(InvStr()),
      _isAlarm(false),
      _rowNR(0),
      _tableItemHeight(20)
{
    _nibpNrendCacheMap.clear();
    // 设置标题栏的相关信息。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    setPalette(palette);
    setName(trs(paramInfo.getParamName(PARAM_NIBP)));
    setUnit(Unit::getSymbol(nibpParam.getUnit()));

    QColor color = colorManager.getColor(paramInfo.getParamName(PARAM_NIBP));
    QString Style = QString("background-color:transparent;"
                            "color:rgb(%1,%2,%3);")
                    .arg(color.red()).arg(color.green()).arg(color.blue());
    QString headStyle = QString("QHeaderView::section{color:rgb(%1,%2,%3);"
                                "border:0px solid black;"
                                "background-color:black;}")
                    .arg(color.red()).arg(color.green()).arg(color.blue());
    // 开始布局。
    _table = new ITableWidget();
    _table->setFocusPolicy(Qt::NoFocus);                                  // 不聚焦。
    _table->setColumnCount(COLUMN_COUNT);
    _table->verticalHeader()->setVisible(false);                          // 列首隐藏
    _table->horizontalHeader()->setVisible(true);                        // 列首隐藏
    _table->setShowGrid(false);                                           //显示表格线
    _table->setStyleSheet(Style);
    _table->horizontalHeader()->setStyleSheet(headStyle);
    QStringList titleList = QStringList() << trs("Time")
                                        << trs("NIBPList")
                                       << trs("PR");
    _table->setHorizontalHeaderLabels(titleList);

    connect(_table, SIGNAL(released(QMouseEvent *)), this, SLOT(mouseReleaseEvent(QMouseEvent *)));

    _table->setSelectionMode(QAbstractItemView::NoSelection);

    // 布局。
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(10, 0, 10, 0);
    mainLayout->addWidget(_table);
    contentLayout->addLayout(mainLayout);

    // 释放事件。
//    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    setFocusPolicy(Qt::NoFocus);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPDataTrendWidget::~NIBPDataTrendWidget()
{
    _nibpNrendCacheMap.clear();
}
