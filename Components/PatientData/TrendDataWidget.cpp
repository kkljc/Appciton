#include "TrendDataWidget.h"
#include "IButton.h"
#include "IDropList.h"
#include "ITableWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "ParamManager.h"
#include "TimeDate.h"
#include "TrendDataSetWidget.h"
#include "TimeManager.h"
#include "TrendDataStorageManager.h"
#include "IBPParam.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QStylePainter>
#include <QApplication>
#include <QScrollBar>
#include <QTableWidgetItem>
#include "RecorderManager.h"
#include "TrendTablePageGenerator.h"
#include "IConfig.h"
#include "TrendPrintWidget.h"
#include "EventStorageManager.h"
#include "EventDataParseContext.h"

#define ITEM_HEIGHT             30
#define ITEM_WIDTH              100
#define TABLE_ROW_NR            6
#define TABLE_COL_NR            7
#define TABLE_ITEM_WIDTH        65
#define TABLE_ITEM_HEIGHT       35

TrendDataWidget *TrendDataWidget::_selfObj = NULL;

/**********************************************************************************************************************
 * 析构。
 **********************************************************************************************************************/
TrendDataWidget::~TrendDataWidget()
{
    qDeleteAll(_trendDataPack);
    _trendDataPack.clear();
}

/**********************************************************************************************************************
 * 设置IBP对应标名参数是否可见
 **********************************************************************************************************************/
void TrendDataWidget::isIBPSubParamVisible(IBPPressureName name, bool flag)
{
    _ibpNameMap.find(name).value() = flag;
}

/**********************************************************************************************************************
 * 加载趋势数据
 **********************************************************************************************************************/
//void TrendDataWidget::loadTrendData()
//{
//    QString time;
//    QStringList timeTitle;
//    QTableWidgetItem *item;
//    int intervalNum = TrendDataSymbol::convertValue(_timeInterval)/TrendDataSymbol::convertValue(RESOLUTION_RATIO_5_SECOND);
//    int col = 0;
//    QList<int> indexList;
//    for (int i = _trendDataPack.length() - 1 - intervalNum * _currentMoveCount; i >= 0;
//         i = i - intervalNum)
//    {
//        if (col == TABLE_COL_NR)
//        {
//            break;
//        }
//        timeDate.getTime(_trendDataPack.at(i)->time, time, true);
//        timeTitle << time;

//        if (_trendDataPack.at(i)->alarmFlag)
//        {
//            indexList.append(col);
//        }
//        for (int j = 0; j < _curDisplayParamRow; j ++)
//        {
//            item = table->item(j, col);
//            if (!item)
//            {
//                item = new QTableWidgetItem();
//                item->setTextAlignment(Qt::AlignCenter);
//                table->setItem(j, col, item);
//            }

//            if (_displayList.at(j) == SUB_PARAM_NIBP_MAP)
//            {
//                short nibpSys = _trendDataPack.at(i)->subparamValue.value((SubParamID)(_displayList.at(j) - 2), InvData());
//                short nibpDia = _trendDataPack.at(i)->subparamValue.value((SubParamID)(_displayList.at(j) - 1), InvData());
//                short nibpMap = _trendDataPack.at(i)->subparamValue.value(_displayList.at(j), InvData());
//                QString sysStr = nibpSys == InvData() ?"---":QString::number(nibpSys);
//                QString diaStr = nibpDia == InvData() ?"---":QString::number(nibpDia);
//                QString mapStr = nibpMap == InvData() ?"---":QString::number(nibpMap);
//                QString nibpStr = sysStr + "/" + diaStr + "(" + mapStr + ")";
//                item->setText(nibpStr);
//            }
//            else if (_displayList.at(j) == SUB_PARAM_ART_MAP || _displayList.at(j) == SUB_PARAM_PA_MAP ||
//                     _displayList.at(j) == SUB_PARAM_AUXP1_MAP || _displayList.at(j) == SUB_PARAM_AUXP2_MAP)
//            {
//                short ibpSys = _trendDataPack.at(i)->subparamValue.value((SubParamID)(_displayList.at(j) - 2), InvData());
//                short ibpDia = _trendDataPack.at(i)->subparamValue.value((SubParamID)(_displayList.at(j) - 1), InvData());
//                short ibpMap = _trendDataPack.at(i)->subparamValue.value(_displayList.at(j), InvData());
//                QString sysStr = ibpSys == InvData() ?"---":QString::number(ibpSys);
//                QString diaStr = ibpDia == InvData() ?"---":QString::number(ibpDia);
//                QString mapStr = ibpMap == InvData() ?"---":QString::number(ibpMap);
//                QString ibpStr = sysStr + "/" + diaStr + "(" + mapStr + ")";
//                item->setText(ibpStr);
//            }
//            else if (_displayList.at(j) == SUB_PARAM_ETCO2 || _displayList.at(j) == SUB_PARAM_ETN2O ||
//                     _displayList.at(j) == SUB_PARAM_ETAA1 || _displayList.at(j) == SUB_PARAM_ETAA2 ||
//                     _displayList.at(j) == SUB_PARAM_ETO2 || _displayList.at(j) == SUB_PARAM_T1)
//            {
//                short data1 = _trendDataPack.at(i)->subparamValue.value(_displayList.at(j), InvData());
//                short data2 = _trendDataPack.at(i)->subparamValue.value((SubParamID)(_displayList.at(j) + 1), InvData());
//                QString dataStr1 = data1 == InvData() ?"---":QString::number(data1);
//                QString dataStr2 = data2 == InvData() ?"---":QString::number(data2);
//                QString dataStr = dataStr1 + "/" + dataStr2;
//                item->setText(dataStr);
//            }
//            else
//            {
//                short data = _trendDataPack.at(i)->subparamValue.value(_displayList.at(j), InvData());
//                QString dataStr = data == InvData() ?"---":QString::number(data);
//                item->setText(dataStr);
//            }

//            bool isAlarm = _trendDataPack.at(i)->subparamAlarm.value(_displayList.at(j), false);
//            if (isAlarm)
//            {
//                item->setBackgroundColor(Qt::yellow);
//            }
//            else
//            {
//                item->setBackgroundColor(Qt::white);
//            }
//        }
//        col ++;
//    }

//    for (timeTitle.length(); timeTitle.length() < TABLE_COL_NR;)
//    {
//        timeTitle << "";
//    }

//    if ((_trendDataPack.length() - TABLE_COL_NR) < 0)
//    {
//        _hideColumn = 0;
//    }
//    else
//    {
//        _hideColumn = _trendDataPack.length() - TABLE_COL_NR;
//    }
//    table->setHorizontalHeaderLabels(timeTitle);

//    for (int i = 0; i < indexList.count(); i ++)
//    {
//        item = table->horizontalHeaderItem(indexList.at(i));
//        item->setBackgroundColor(Qt::yellow);
//    }
//}

void TrendDataWidget::loadTrendData()
{
    QString time;
    QStringList timeTitle;
    QTableWidgetItem *item;
    int  timeInterval = TrendDataSymbol::convertValue(_timeInterval);
    int col = 0;
    QList<int> indexList;
    int startIndex;
    int endIndex;
    _dataIndex(startIndex, endIndex);
    bool isStart = true;
    if (startIndex != InvData() && endIndex != InvData())
    {
        unsigned lastTime = _trendDataPack.at(startIndex)->time;
        TrendDataPackage *pack;
        for (int i = startIndex; i <= endIndex; i ++)
        {
            pack = _trendDataPack.at(i);
            unsigned t = pack->time;

            // 判断是否有事件报警触发
            if (pack->alarmFlag)
            {
                indexList.append(col);
            }

            if (t != lastTime)
            {
                continue;
            }

            timeDate.getTime(t, time, true);
            timeTitle << time;

            // 取出一屏的开始到结束时间
            _endTime = t;
            if (isStart)
            {
                _startTime = t;
                isStart = false;
            }

            for (int j = 0; j < _curDisplayParamRow; j ++)
            {
                item = table->item(j, col);
                if (!item)
                {
                    item = new QTableWidgetItem();
                    item->setTextAlignment(Qt::AlignCenter);
                    table->setItem(j, col, item);
                }

                if (_displayList.at(j) == SUB_PARAM_NIBP_MAP)
                {
                    short nibpSys = pack->subparamValue.value((SubParamID)(_displayList.at(j) - 2), InvData());
                    short nibpDia = pack->subparamValue.value((SubParamID)(_displayList.at(j) - 1), InvData());
                    short nibpMap = pack->subparamValue.value(_displayList.at(j), InvData());
                    QString sysStr = nibpSys == InvData() ?"---":QString::number(nibpSys);
                    QString diaStr = nibpDia == InvData() ?"---":QString::number(nibpDia);
                    QString mapStr = nibpMap == InvData() ?"---":QString::number(nibpMap);
                    QString nibpStr = sysStr + "/" + diaStr + "(" + mapStr + ")";
                    item->setText(nibpStr);
                }
                else if (_displayList.at(j) == SUB_PARAM_ART_MAP || _displayList.at(j) == SUB_PARAM_PA_MAP ||
                         _displayList.at(j) == SUB_PARAM_AUXP1_MAP || _displayList.at(j) == SUB_PARAM_AUXP2_MAP)
                {
                    short ibpSys = pack->subparamValue.value((SubParamID)(_displayList.at(j) - 2), InvData());
                    short ibpDia = pack->subparamValue.value((SubParamID)(_displayList.at(j) - 1), InvData());
                    short ibpMap = pack->subparamValue.value(_displayList.at(j), InvData());
                    QString sysStr = ibpSys == InvData() ?"---":QString::number(ibpSys);
                    QString diaStr = ibpDia == InvData() ?"---":QString::number(ibpDia);
                    QString mapStr = ibpMap == InvData() ?"---":QString::number(ibpMap);
                    QString ibpStr = sysStr + "/" + diaStr + "(" + mapStr + ")";
                    item->setText(ibpStr);
                }
                else if (_displayList.at(j) == SUB_PARAM_ETCO2 || _displayList.at(j) == SUB_PARAM_ETN2O ||
                         _displayList.at(j) == SUB_PARAM_ETAA1 || _displayList.at(j) == SUB_PARAM_ETAA2 ||
                         _displayList.at(j) == SUB_PARAM_ETO2 || _displayList.at(j) == SUB_PARAM_T1)
                {
                    short data1 = pack->subparamValue.value(_displayList.at(j), InvData());
                    short data2 = pack->subparamValue.value((SubParamID)(_displayList.at(j) + 1), InvData());
                    QString dataStr1 = data1 == InvData() ?"---":QString::number(data1);
                    QString dataStr2 = data2 == InvData() ?"---":QString::number(data2);
                    QString dataStr = dataStr1 + "/" + dataStr2;
                    item->setText(dataStr);
                }
                else
                {
                    short data = pack->subparamValue.value(_displayList.at(j), InvData());
                    QString dataStr = data == InvData() ?"---":QString::number(data);
                    item->setText(dataStr);
                }

                bool isAlarm = pack->subparamAlarm.value(_displayList.at(j), false);
                if (isAlarm)
                {
                    item->setBackgroundColor(Qt::yellow);
                }
                else
                {
                    item->setBackgroundColor(Qt::white);
                }
            }
            col ++;
            lastTime = lastTime + timeInterval;
        }
    }

    // 当数据不够一屏时,后面补空
    for (; col < TABLE_COL_NR; col ++)
    {
        timeTitle << "";
        for (int j = 0; j < _curDisplayParamRow; j ++)
        {
            item = table->item(j, col);
            if (!item)
            {
                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                table->setItem(j, col, item);
            }
            item->setText("");
            item->setBackgroundColor(Qt::white);
        }
    }

    for (timeTitle.length(); timeTitle.length() < TABLE_COL_NR;)
    {
        timeTitle << "";
    }

    if ((_trendDataPack.length() - TABLE_COL_NR) < 0)
    {
        _hideColumn = 0;
    }
    else
    {
        _hideColumn = _trendDataPack.length() - TABLE_COL_NR;
    }
    table->setHorizontalHeaderLabels(timeTitle);

    for (int i = 0; i < TABLE_COL_NR; i ++)
    {
        item = table->horizontalHeaderItem(i);
        item->setBackgroundColor(Qt::white);
    }
    for (int i = 0; i < indexList.count(); i ++)
    {

        item = table->horizontalHeaderItem(indexList.at(i));
        item->setBackgroundColor(Qt::yellow);
    }
}

void TrendDataWidget::showEvent(QShowEvent *event)
{
    PopupWidget::showEvent(event);
    _loadTableTitle();
}

/**********************************************************************************************************************
 * 趋势数据设置槽函数
 **********************************************************************************************************************/
void TrendDataWidget::_trendDataSetReleased()
{
    trendDataSetWidget.autoShow();
    hide();
}

/**********************************************************************************************************************
 * 上翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_upReleased()
{
    int maxValue = table->verticalScrollBar()->maximum();
    _curVScroller = table->verticalScrollBar()->value();

    if(_curVScroller>0)
    {
        int position = _curVScroller-(maxValue * TABLE_ROW_NR)/(_curDisplayParamRow - TABLE_ROW_NR);
        table->verticalScrollBar()->setSliderPosition(position);
    }
}

/**********************************************************************************************************************
 * 下翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_downReleased()
{
    int maxValue = table->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值25
    _curVScroller = table->verticalScrollBar()->value(); //获得当前scroller值

    if(_curVScroller<maxValue)
    {
        int position = _curVScroller + (maxValue * TABLE_ROW_NR)/(_curDisplayParamRow - TABLE_ROW_NR);
        table->verticalScrollBar()->setSliderPosition(position);
    }
}

/**********************************************************************************************************************
 * 左翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_leftReleased()
{
    if (_trendDataPack.count() != 0)
    {
        unsigned timeInterval = TrendDataSymbol::convertValue(_timeInterval);
        _leftTime = _leftTime - timeInterval * 4;

        unsigned startTime = _trendDataPack.first()->time;
        if (startTime % timeInterval != 0)
        {
            startTime = startTime + (timeInterval - startTime % timeInterval);
        }

        if (_leftTime < startTime)
        {
            _leftTime = startTime;
        }
        _rightTime = _leftTime + timeInterval * (TABLE_COL_NR - 1);
        loadTrendData();
    }
}

/**********************************************************************************************************************
 * 右翻页。
 **********************************************************************************************************************/
void TrendDataWidget::_rightReleased()
{
    if (_trendDataPack.count() != 0)
    {
        unsigned timeInterval = TrendDataSymbol::convertValue(_timeInterval);
        _rightTime = _rightTime + timeInterval * 4;
        unsigned endTime = _trendDataPack.last()->time;
        endTime = endTime - endTime % timeInterval;
        if (_rightTime > endTime)
        {
            _rightTime = endTime;
        }
        _leftTime = _rightTime - timeInterval * (TABLE_COL_NR - 1);
        loadTrendData();
    }
}

void TrendDataWidget::_printWidgetRelease()
{
    if (_trendDataPack.count() != 0)
    {
        TrendPrintWidget printWidget;
        unsigned startLimit = _trendDataPack.first()->time;
        unsigned endLimit = _trendDataPack.last()->time;
        printWidget.printTimeRange(startLimit, endLimit);
        printWidget.initPrintTime(_startTime, _endTime);
        printWidget.exec();
    }
    //TEST
//    IStorageBackend *backend = trendDataStorageManager.backend();
//    if(backend->getBlockNR() <= 0)
//    {
//        return;
//    }
//    RecordPageGenerator *gen= new TrendTablePageGenerator(backend, 0, backend->getBlockNR() - 1);
//    recorderManager.addPageGenerator(gen);
}

/**********************************************************************************************************************
 * 构造。
 **********************************************************************************************************************/
TrendDataWidget::TrendDataWidget() : _timeInterval(RESOLUTION_RATIO_5_SECOND), _hideColumn(0)
{
    setTitleBarText(trs("TrendTable"));
    _trendParamInit();

    _maxWidth = windowManager.getPopMenuWidth();
    _maxHeight = windowManager.getPopMenuHeight();

    int fontSize = fontManager.getFontSize(1);
    setFont(fontManager.textFont(fontSize));

    table = new ITableWidget();
    table->setColumnCount(TABLE_COL_NR);
    table->setFocusPolicy(Qt::NoFocus);
    table->setFixedHeight(ITEM_HEIGHT * 7 * 2 + table->horizontalHeader()->height());
    table->setFixedWidth(_maxWidth - 10);
    table->horizontalHeader()->setVisible(true);
    table->verticalHeader()->setVisible(true);
    table->verticalHeader()->setStyleSheet("QHeaderView::section {border:0px;height:46px;}");
    table->verticalHeader()->setFixedWidth(ITEM_WIDTH);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setSelectionBehavior(QAbstractItemView::SelectColumns);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setClickable(false);
    table->setStyleSheet("QTableView { border: none;"
                             "background-color: rgb(255, 255, 255, 255);"
                             "selection-background-color: rgb(65, 105, 225, 200);"
                             "selection-color: black;"
                             "font: 10pt;}");

    // 左上角的按钮（表头交叉处）
    label = new QLabel(table);
    label->setFixedWidth(ITEM_WIDTH);

    _up = new IButton();
    _up->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    _left = new IButton();
    _left->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _left->setPicture(QImage("/usr/local/nPM/icons/ArrowLeft.png"));
    connect(_left, SIGNAL(realReleased()), this, SLOT(_leftReleased()));

    _right = new IButton();
    _right->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _right->setPicture(QImage("/usr/local/nPM/icons/ArrowRight.png"));
    connect(_right, SIGNAL(realReleased()), this, SLOT(_rightReleased()));

    _incidentMove = new IButton(trs("IncidentMove"));
    _incidentMove->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _incidentMove->setFont(fontManager.textFont(fontSize));

    _incident = new IDropList(trs("Incident"));
    _incident->setFont(fontManager.textFont(fontSize));
    _incident->setFixedHeight(ITEM_HEIGHT);
    _incident->setMinimumWidth(ITEM_WIDTH);
    _incident->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    _printParam = new IDropList(trs("Print"));
    _printParam->setFont(fontManager.textFont(fontSize));
    _printParam->setFixedHeight(ITEM_HEIGHT);
    _printParam->setMinimumWidth(ITEM_WIDTH);
    _printParam->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    connect(_printParam, SIGNAL(realReleased()), this, SLOT(_printWidgetRelease()));

    _set = new IButton(trs("Set"));
    _set->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _set->setFont(fontManager.textFont(fontSize));
    connect(_set, SIGNAL(realReleased()), this, SLOT(_trendDataSetReleased()));

    QHBoxLayout *lineLayout = new QHBoxLayout();
    lineLayout->setMargin(0);
    lineLayout->setSpacing(2);
    lineLayout->addStretch();
    lineLayout->addWidget(_up);
    lineLayout->addWidget(_down);
    lineLayout->addWidget(_left);
    lineLayout->addWidget(_right);
    lineLayout->addWidget(_incidentMove);
    lineLayout->addWidget(_incident);
    lineLayout->addWidget(_printParam);
    lineLayout->addWidget(_set);
    lineLayout->addStretch();

    contentLayout->addStretch();
    contentLayout->addWidget(table);
    contentLayout->addStretch();
    contentLayout->addLayout(lineLayout);

    setFixedSize(_maxWidth, _maxHeight);

    _curDate.clear();

    QString prefix = "TrendTable|";
    int index = 0;
    QString ratioPrefix = prefix + "ResolutionRatio";
    systemConfig.getNumValue(ratioPrefix, index);
    _timeInterval = (ResolutionRatio)index;
    index = 0;
    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    loadCurParam(index);
    _updateHeaderDate();
}

/**************************************************************************************************
 * 载入表格表头信息。
 *************************************************************************************************/
void TrendDataWidget::_loadTableTitle()
{
    QStringList hheader;
    QString str("");

    table->clear();
    _displayList.clear();

    label->setText(_curDate);
    str.clear();

    _curDisplayParamRow = 0;
    for (int i = 0; i < _curList.length(); i ++)
    {
        SubParamID id = _curList.at(i);

        if (id  == SUB_PARAM_NIBP_MAP)
        {
            str = trs(paramInfo.getParamName(PARAM_NIBP));
        }
        else if(id == SUB_PARAM_ART_MAP || id == SUB_PARAM_PA_MAP ||
                id == SUB_PARAM_CVP_MAP || id == SUB_PARAM_LAP_MAP ||
                id == SUB_PARAM_RAP_MAP || id == SUB_PARAM_ICP_MAP ||
                id == SUB_PARAM_AUXP1_MAP || id == SUB_PARAM_AUXP2_MAP)
        {
            if (_ibpNameMap.value(ibpParam.getPressureName(id), false))
            {
                str = paramInfo.getSubParamName(id);
                str = str.left(str.length() - 4);
            }
            else
            {
                continue;
            }
        }
        else
        {
            str = trs(paramInfo.getSubParamName(id));
        }
        str += '\n';
        str += "(";
        str += Unit::localeSymbol(paramManager.getSubParamUnit(paramInfo.getParamID(id), id));
        str += ")";
        _curDisplayParamRow ++;
        hheader << str;
        _displayList.append(id);

    }

    table->setRowCount(_curDisplayParamRow);
    for (int i = 0; i < _curDisplayParamRow; i++)
    {
        table->setRowHeight(i, TABLE_ITEM_HEIGHT * 2);
    }
    table->setVerticalHeaderLabels(hheader);

    _getTrendData();
    _updateDisplayTime();
    loadTrendData();
}

/**************************************************************************************************
 * 载入当前显示参数。
 *************************************************************************************************/
void TrendDataWidget::loadCurParam(int index)
{
    _curList.clear();

    TrendParamList list;

    if (_orderMap.contains(PARAM_DUP_ECG))
    {
        list = _orderMap.values(PARAM_DUP_ECG);
        qSort(list);
        _curList.append(list);
    }

    if (_orderMap.contains(PARAM_SPO2))
    {
        list = _orderMap.values(PARAM_SPO2);
        qSort(list);
        _curList.append(list);
    }

    if (_orderMap.contains(PARAM_NIBP))
    {
        list = _orderMap.values(PARAM_NIBP);
        qSort(list);
        _curList.append(list);
    }

    if (_orderMap.contains(PARAM_TEMP))
    {
        list = _orderMap.values(PARAM_TEMP);
        qSort(list);
        _curList.append(list);
    }

    if ((TrendGroup)index == TREND_GROUP_RESP)
    {
        if (_orderMap.contains(PARAM_DUP_RESP))
        {
            list = _orderMap.values(PARAM_DUP_RESP);
            qSort(list);
            _curList.append(list);
        }

        if (_orderMap.contains(PARAM_CO2))
        {
            list = _orderMap.values(PARAM_CO2);
            qSort(list);
            _curList.append(list);
        }
    }
    else if ((TrendGroup)index == TREND_GROUP_IBP)
    {
        if (_orderMap.contains(PARAM_IBP))
        {
            list = _orderMap.values(PARAM_IBP);
            qSort(list);
            _curList.append(list);
        }

        if (_orderMap.contains(PARAM_CO))
        {
            list = _orderMap.values(PARAM_CO);
            qSort(list);
            _curList.append(list);
        }
    }
    else if ((TrendGroup)index == TREND_GROUP_AG)
    {
        if (_orderMap.contains(PARAM_AG))
        {
            list = _orderMap.values(PARAM_AG);
            qSort(list);
            _curList.append(list);
        }
    }
}

/**********************************************************************************************************************
 * 设置时间间隔
 **********************************************************************************************************************/
void TrendDataWidget::setTimeInterval(ResolutionRatio flag)
{
    _timeInterval = flag;
}

void TrendDataWidget::printTrendData(unsigned startTime, unsigned endTime)
{
    int startIndex;
    int endIndex;

    // 二分查找时间索引
    int lowPos = 0;
    int highPos = _trendDataPack.count() - 1;
    while(lowPos <= highPos)
    {
        int midPos = (lowPos + highPos)/2;
        int timeDiff = qAbs(startTime - _trendDataPack.at(midPos)->time);

        if (startTime < _trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (startTime > _trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            startIndex = midPos;
            break;
        }
    }

    lowPos = 0;
    highPos = _trendDataPack.count() - 1;
    while(lowPos <= highPos)
    {
        int midPos = (lowPos + highPos)/2;
        int timeDiff = qAbs(endTime - _trendDataPack.at(midPos)->time);

        if (endTime < _trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (endTime > _trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            endIndex = midPos;
            break;
        }
    }

    // 打印
    IStorageBackend *backend = trendDataStorageManager.backend();
    if(backend->getBlockNR() <= 0)
    {
        return;
    }
    RecordPageGenerator *gen= new TrendTablePageGenerator(backend, startIndex, endIndex, TrendDataSymbol::convertValue(_timeInterval));
    recorderManager.addPageGenerator(gen);
}

/**********************************************************************************************************************
 * 趋势参数初始化。
 **********************************************************************************************************************/
void TrendDataWidget::_trendParamInit()
{
    _ibpNameMap.clear();

    for (int i = 0; i < IBP_PRESSURE_NR; i ++)
    {
        _ibpNameMap.insert((IBPPressureName)i, false);
    }

    _orderMap.clear();

    QList<ParamID> paramIDList;
    paramManager.getParams(paramIDList);
    qSort(paramIDList);

    int count = 0;
    for (int i = 0; i < SUB_PARAM_NR; i ++)
    {
        ParamID paramID = paramInfo.getParamID((SubParamID)i);
        if (-1 == paramIDList.indexOf(paramID))
        {
            continue;
        }

        switch (i)
        {
        case SUB_PARAM_ECG_PVCS:
        case SUB_PARAM_ST_I:
        case SUB_PARAM_ST_II:
        case SUB_PARAM_ST_III:
        case SUB_PARAM_ST_aVR:
        case SUB_PARAM_ST_aVL:
        case SUB_PARAM_ST_aVF:
        case SUB_PARAM_ST_V1:
        case SUB_PARAM_ST_V2:
        case SUB_PARAM_ST_V3:
        case SUB_PARAM_ST_V4:
        case SUB_PARAM_ST_V5:
        case SUB_PARAM_ST_V6:
        case SUB_PARAM_NIBP_SYS:
        case SUB_PARAM_NIBP_DIA:
        case SUB_PARAM_ART_DIA:
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_ART_PR:
        case SUB_PARAM_PA_DIA:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_PA_PR:
        case SUB_PARAM_CVP_PR:
        case SUB_PARAM_LAP_PR:
        case SUB_PARAM_RAP_PR:
        case SUB_PARAM_ICP_PR:
        case SUB_PARAM_AUXP1_DIA:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP1_PR:
        case SUB_PARAM_AUXP2_DIA:
        case SUB_PARAM_AUXP2_SYS:
        case SUB_PARAM_AUXP2_PR:
        case SUB_PARAM_FICO2:
        case SUB_PARAM_FIN2O:
        case SUB_PARAM_FIAA1:
        case SUB_PARAM_FIAA2:
        case SUB_PARAM_FIO2:
        case SUB_PARAM_T2:
        case SUB_PARAM_TD:
            break;
        default:
        {
            ++ count;
            _orderMap.insert(paramID, (SubParamID)i);
            break;
        }
        }
    }
}

/**********************************************************************************************************************
 * 当前日期。
 **********************************************************************************************************************/
void TrendDataWidget::_updateHeaderDate(unsigned t)
{
    if (0 == t)
    {
        t = timeManager.getCurTime();
    }

    QString updateDate;
    timeDate.getDate(t, updateDate, true);

    if (_curDate != updateDate)
    {
        QTableWidgetItem *item = table->verticalHeaderItem(0);
        if (NULL != item)
        {
            item->setText(updateDate);
        }

        _curDate = updateDate;
    }
}

/**********************************************************************************************************************
 * 获取趋势数据
 **********************************************************************************************************************/
void TrendDataWidget::_getTrendData()
{
    // 趋势数据
    IStorageBackend *backend;
    backend = trendDataStorageManager.backend();
    int blockNum = backend->getBlockNR();
    QByteArray data;
    TrendDataSegment *dataSeg;
    TrendDataPackage *pack;
    qDeleteAll(_trendDataPack);
    _trendDataPack.clear();
    for (int i = 0; i < blockNum; i ++)
    {
        pack = new TrendDataPackage;
        data = backend->getBlockData((quint32)i);
        dataSeg = (TrendDataSegment*)data.data();
        pack->time = dataSeg->timestamp;
        pack->co2Baro = dataSeg->co2Baro;
        for (int j = 0; j < dataSeg->trendValueNum; j ++)
        {
            pack->subparamValue[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].value;
            pack->subparamAlarm[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].alarmFlag;
//            if (!pack->alarmFlag && dataSeg->values[j].alarmFlag)
//            {
//                pack->alarmFlag = dataSeg->values[j].alarmFlag;
//            }
            pack->alarmFlag = dataSeg->eventFlag;
        }
        _trendDataPack.append(pack);
    }
}

void TrendDataWidget::_dataIndex(int &startIndex, int &endIndex)
{
    // 开始和结尾的索引查找
    startIndex = InvData();
    endIndex = InvData();

    // 二分查找时间索引
    int lowPos = 0;
    int highPos = _trendDataPack.count() - 1;
    while(lowPos <= highPos)
    {
        int midPos = (lowPos + highPos)/2;
        int timeDiff = qAbs(_leftTime - _trendDataPack.at(midPos)->time);

        if (_leftTime < _trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (_leftTime > _trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            startIndex = midPos;
            break;
        }
    }

    lowPos = 0;
    highPos = _trendDataPack.count() - 1;
    while(lowPos <= highPos)
    {
        int midPos = (lowPos + highPos)/2;
        int timeDiff = qAbs(_rightTime - _trendDataPack.at(midPos)->time);

        if (_rightTime < _trendDataPack.at(midPos)->time)
        {
            highPos = midPos - 1;
        }
        else if (_rightTime > _trendDataPack.at(midPos)->time)
        {
            lowPos = midPos + 1;
        }

        if (timeDiff == 0 || lowPos > highPos)
        {
            endIndex = midPos;
            break;
        }
    }
}

void TrendDataWidget::_updateDisplayTime()
{
    unsigned t;
    unsigned timeInterval = TrendDataSymbol::convertValue(_timeInterval);
    if (_trendDataPack.count() != 0)
    {
        unsigned lastTime = _trendDataPack.last()->time;
        t = lastTime - lastTime % timeInterval;
        _rightTime = t;
        _leftTime = t - timeInterval * (TABLE_COL_NR - 1);
    }
}
