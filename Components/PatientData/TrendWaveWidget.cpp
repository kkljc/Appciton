#include "TrendWaveWidget.h"
#include "TrendSubWaveWidget.h"
#include "TimeDate.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "TrendGraphSetWidget.h"
#include "IBPParam.h"
#include "TrendDataStorageManager.h"
#include "TrendDataSymbol.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>

#define GRAPH_DISPLAY_DATA_NUMBER           4
#define GRAPH_POINT_NUMBER                  120                     // 一屏数据量
#define DATA_INTERVAL_PIXEL                 5                       // 两数据之间的像素点个数
#define GRAPH_DATA_WIDTH                    600                     // 一屏数据所占像素点

TrendWaveWidget::TrendWaveWidget() :
    _timeInterval(RESOLUTION_RATIO_5_SECOND),
    _initTime(0),
    _cursorPosIndex(0), _currentCursorTime(0),
    _displayGraphNum(3), _totalGraphNum(3), _dataSize(0)
{
    _initTime = timeDate.time();
    _initTime = _initTime - _initTime % 5;
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::NoFocus);

    _subWidget = new IWidget();
    _subWidget->setFocusPolicy(Qt::NoFocus);
    _subWidgetScrollArea = new QScrollArea();

    // 重新调整子部件的大小
    _subWidgetScrollArea->setWidgetResizable(true);

    // 滚动区域透明处理
    _subWidgetScrollArea->setStyleSheet("QScrollArea {background-color:transparent;}");
    _subWidgetScrollArea->viewport()->setStyleSheet("background-color:transparent;");

    _subWidgetScrollArea->setFocusPolicy(Qt::NoFocus);

    _subWidgetScrollArea->verticalScrollBar()->setStyleSheet(
                                                             "QScrollBar:vertical"
                                                             "{"
                                                             "width:12px;"
                                                             "}"
                                                             );

    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(0);

    _hLayoutTrend = new QVBoxLayout();
    _hLayoutTrend->setMargin(0);
    _hLayoutTrend->setSpacing(0);
    _subWidget->setLayout(_hLayoutTrend);

    _subWidgetScrollArea->setWidget(_subWidget);

    _mainLayout->addSpacing(30);
    _mainLayout->addWidget(_subWidgetScrollArea);

    setLayout(_mainLayout);

//    _getTrendData();

}

TrendWaveWidget::~TrendWaveWidget()
{

}

/**************************************************************************************************
 * 切换趋势数据的显示
 *************************************************************************************************/
void TrendWaveWidget::changeTrendDisplay()
{
    _getTrendData();
    _trendLayout();
}

/**************************************************************************************************
 * 设置窗口大小
 *************************************************************************************************/
void TrendWaveWidget::setWidgetSize(int w, int h)
{
    setFixedSize(w, h);
    _waveRegionWidth = width()/5 * 4;
    _oneFrameWidth = GRAPH_DATA_WIDTH/GRAPH_DISPLAY_DATA_NUMBER;
    //确认好窗口大小后初始化子波形窗口
    _initWaveSubWidget();
}

/**************************************************************************************************
 * 左移坐标
 *************************************************************************************************/
void TrendWaveWidget::leftMoveCoordinate()
{
    if (_currentPage >= _totalPage)
    {
        return;
    }
    else
    {
        _currentPage ++;
    }
//    _cursorPosition = GRAPH_POINT_NUMBER;
//    _clearLayout();
    _trendLayout();

    update();
}

/**************************************************************************************************
 * 右移坐标
 *************************************************************************************************/
void TrendWaveWidget::rightMoveCoordinate()
{
    if (_currentPage <= 1)
    {
        return;
    }
    else
    {
        _currentPage --;
    }
//    _cursorPosition = GRAPH_POINT_NUMBER;
//    _clearLayout();
    _trendLayout();

    update();

}

/**************************************************************************************************
 * 左移游标
 *************************************************************************************************/
void TrendWaveWidget::leftMoveCursor()
{
    if (_cursorPosIndex <= 0)
    {
        _cursorPosIndex = 0;
    }
    else{
        _cursorPosIndex --;
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosIndex);
        }
    }
    update();
}

/**************************************************************************************************
 * 右移游标
 *************************************************************************************************/
void TrendWaveWidget::rightMoveCursor()
{
    if (_cursorPosIndex >= _trendGraphInfo.trendData.count() - 1)
    {
        _cursorPosIndex = _trendGraphInfo.trendData.count() - 1;
    }
    else{
        _cursorPosIndex ++;
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosIndex);
        }
    }
    update();
}

/**************************************************************************************************
 * 参数上翻页
 *************************************************************************************************/
void TrendWaveWidget::pageUpParam()
{
    int maxValue = _subWidgetScrollArea->verticalScrollBar()->maximum();
    _curVScroller = _subWidgetScrollArea->verticalScrollBar()->value();
    if (_curVScroller > 0)
    {
        QScrollBar *scrollBar = _subWidgetScrollArea->verticalScrollBar();
        // 当前位置　－　最大位置乘以总行数除以看不见的行数
        int positon = _curVScroller - (maxValue * _displayGraphNum) / (_totalGraphNum - _displayGraphNum);
        scrollBar->setSliderPosition(positon);
    }

}

/**************************************************************************************************
 * 参数下翻页
 *************************************************************************************************/
void TrendWaveWidget::pageDownParam()
{
    int maxValue = _subWidgetScrollArea->verticalScrollBar()->maximum();
    _curVScroller = _subWidgetScrollArea->verticalScrollBar()->value();
    if (_curVScroller < maxValue && _totalGraphNum != _displayGraphNum)
    {
        QScrollBar *scrollBar = _subWidgetScrollArea->verticalScrollBar();
        int positon = _curVScroller + (maxValue * _displayGraphNum) / (_totalGraphNum - _displayGraphNum);
        scrollBar->setSliderPosition(positon);
    }
}

void TrendWaveWidget::updateTrendGraph()
{
    changeTrendDisplay();
    update();
}

void TrendWaveWidget::setTimeInterval(ResolutionRatio timeInterval)
{
    _timeInterval = timeInterval;
    _cursorPosIndex = GRAPH_POINT_NUMBER;
    changeTrendDisplay();
    update();
}

/**************************************************************************************************
 * 设置波形数目
 *************************************************************************************************/
void TrendWaveWidget::setWaveNumber(int num)
{
    _displayGraphNum = num;    
    changeTrendDisplay();
}

/**************************************************************************************************
 * 设置标尺限
 *************************************************************************************************/
void TrendWaveWidget::setRulerLimit(SubParamID id, int down, int up)
{
    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i ++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = qobject_cast<TrendSubWaveWidget *>(item->widget());
        if (widget->getSubParamID() == id)
        {
            widget->setRulerRange(down, up);
            return;
        }
    }
}

/**************************************************************************************************
 * 载入趋势数据到数组
 *************************************************************************************************/
void TrendWaveWidget::loadTrendData(SubParamID subID)
{
    // 开始和结尾的索引查找
    int startIndex = InvData();
    int endIndex = InvData();

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

    if (startIndex == InvData() || endIndex == InvData())
    {
        return;
    }

    _trendGraphInfo.reset();
    switch (subID)
    {
    case SUB_PARAM_HR_PR:
    case SUB_PARAM_SPO2:
    case SUB_PARAM_RR_BR:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_ICP_MAP:
    {
        TrendGraphData dataV1;
        for(int i = startIndex; i <= endIndex; i ++)
        {
            dataV1.data = _trendDataPack.at(i)->subparamValue.value(subID, InvData());
            dataV1.timestamp = _trendDataPack.at(i)->time;
            _trendGraphInfo.trendData.append(dataV1);
            _trendGraphInfo.subParamID = subID;
        }
        break;
    }
    case SUB_PARAM_NIBP_MAP:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP2_MAP:
    {        
        TrendGraphData dataV1;
        TrendGraphDataV3 dataV3;
        for(int i = startIndex; i <= endIndex; i ++)
        {
            dataV3.data[0] = _trendDataPack.at(i)->subparamValue.value(subID, InvData());
            dataV3.data[1] = _trendDataPack.at(i)->subparamValue.value(SubParamID(subID - 1), InvData());
            dataV3.data[2] = _trendDataPack.at(i)->subparamValue.value(SubParamID(subID - 2), InvData());
            dataV3.timestamp = _trendDataPack.at(i)->time;            
            dataV1.timestamp = _trendDataPack.at(i)->time;
            _trendGraphInfo.trendData.append(dataV1);
            _trendGraphInfo.trendDataV3.append(dataV3);
            _trendGraphInfo.subParamID = subID;
        }
        break;
    }
    case SUB_PARAM_ETCO2:
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_ETO2:
    case SUB_PARAM_T1:
    {
        TrendGraphData dataV1;
        TrendGraphDataV2 dataV2;
        for(int i = startIndex; i <= endIndex; i ++)
        {
            dataV2.data[0] = _trendDataPack.at(i)->subparamValue.value(subID, InvData());
            dataV2.data[1] = _trendDataPack.at(i)->subparamValue.value(SubParamID(subID + 1), InvData());
            dataV2.timestamp = _trendDataPack.at(i)->time;
            dataV1.timestamp = _trendDataPack.at(i)->time;
            _trendGraphInfo.trendData.append(dataV1);
            _trendGraphInfo.trendDataV2.append(dataV2);
            _trendGraphInfo.subParamID = subID;
        }
        break;
    }
    default:
        break;
    }

    _cursorPosIndex = _trendGraphInfo.trendData.count() - 1;
}

/**************************************************************************************************
 * 主窗口绘图事件
 *************************************************************************************************/
void TrendWaveWidget::paintEvent(QPaintEvent *event)
{
    IWidget::paintEvent(event);

    QPainter barPainter(this);

    barPainter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    QRect rectAdjust =rect().adjusted(0,30,0,0);
    barPainter.drawLine(rectAdjust.topLeft(), rectAdjust.topRight());

//    int intervalNum = TrendDataSymbol::convertValue(_timeInterval)/TrendDataSymbol::convertValue(RESOLUTION_RATIO_5_SECOND);

    double cursorPos;
    if (_trendGraphInfo.trendData.count() - 1 < _cursorPosIndex)
    {
        cursorPos = (_waveRegionWidth + GRAPH_DATA_WIDTH)/2;
    }
    else
    {
        cursorPos = _getCursorPos(_trendGraphInfo.trendData.at(_cursorPosIndex).timestamp);
    }

    unsigned t = _rightTime;
    QString tStr;
    unsigned onePixelTime;
    onePixelTime = TrendDataSymbol::convertValue(_timeInterval);
//    unsigned startT = _rightTime;
    // 坐标刻度
    for (int i = GRAPH_DISPLAY_DATA_NUMBER; i >= 0; i --)
    {
        timeDate.getTime(t, tStr, true);
        barPainter.drawText(rectAdjust.topLeft().x() + (_waveRegionWidth - GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth - 30,
                            rectAdjust.topLeft().y() - 5, tStr);
        t = t - onePixelTime * GRAPH_POINT_NUMBER/GRAPH_DISPLAY_DATA_NUMBER;
        barPainter.drawLine(rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y(), rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y() + 5);
    }

    // 游标线
    barPainter.drawLine(cursorPos, rectAdjust.topLeft().y(),
                        cursorPos, rectAdjust.bottomLeft().y());

    // 当前趋势记录的时间
    if (_cursorPosIndex < _trendGraphInfo.trendData.count())
    {
        timeDate.getDate(_trendGraphInfo.trendData.at(_cursorPosIndex).timestamp, tStr);
        barPainter.drawText(_waveRegionWidth + 5, rectAdjust.topLeft().y() - 5, tStr);
        timeDate.getTime(_trendGraphInfo.trendData.at(_cursorPosIndex).timestamp, tStr, true);
        barPainter.drawText(_waveRegionWidth + 120, rectAdjust.topLeft().y() - 5, tStr);
    }


    barPainter.setPen(QPen(Qt::white, 1, Qt::DotLine));
    barPainter.drawLine(_waveRegionWidth, 0, _waveRegionWidth, height());
}

void TrendWaveWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);
    _getTrendData();
    _cursorPosIndex = 0;

    unsigned t;
    if (_trendDataPack.length() == 0)
    {
        t = _initTime;
    }
    else
    {
        t = _trendDataPack.last()->time;
//        t = _trendDataPack.at(_trendDataPack.length() - 1 - intervalNum * GRAPH_POINT_NUMBER * (_currentPage - 1))->time;
    }
    unsigned onePixelTime;
    onePixelTime = TrendDataSymbol::convertValue(_timeInterval);
    _rightTime = t;
    _leftTime = t - onePixelTime * GRAPH_POINT_NUMBER;
    _trendLayout();
//    update();
}

/**************************************************************************************************
 * 添加显示控件
 *************************************************************************************************/
void TrendWaveWidget::_trendLayout()
{
    int subWidgetHeight = (height() - 30)/_displayGraphNum;
    TrendSubWidgetInfo info;
    info.xHead = (_waveRegionWidth -GRAPH_DATA_WIDTH)/2;
    info.xTail = (_waveRegionWidth + GRAPH_DATA_WIDTH)/2;
    info.yTop = subWidgetHeight/5;
    info.yBottom = subWidgetHeight/5*4;
    _totalGraphNum = 0;
    QMap<SubParamID, TrendSubWaveWidget *>::iterator it = _subWidgetMap.begin();
    for (; it != _subWidgetMap.end(); it ++)
    {
        SubParamID subId = it.key();
        if (trendGraphSetWidget.getTrendGroup() == TREND_GROUP_RESP)
        {
            switch (subId)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_RR_BR:
            case SUB_PARAM_NIBP_MAP:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_T1:                
                loadTrendData(subId);
                it.value()->trendDataInfo(_trendGraphInfo);
                break;
            default:
                it.value()->setVisible(false);
                continue;
            }
        }
        else if(trendGraphSetWidget.getTrendGroup() == TREND_GROUP_IBP)
        {
            SubParamID ibp1;
            SubParamID ibp2;
            ibpParam.getSubParamID(ibp1, ibp2);
            if (subId != ibp1 && subId != ibp2 &&
                    subId != SUB_PARAM_HR_PR && subId != SUB_PARAM_SPO2 &&
                    subId != SUB_PARAM_NIBP_MAP && subId != SUB_PARAM_T1)
            {
                it.value()->setVisible(false);
                continue;
            }
            switch (subId)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_CVP_MAP:
            case SUB_PARAM_LAP_MAP:
            case SUB_PARAM_RAP_MAP:
            case SUB_PARAM_ICP_MAP:
            case SUB_PARAM_NIBP_MAP:
            case SUB_PARAM_T1:
            case SUB_PARAM_ART_MAP:
            case SUB_PARAM_PA_MAP:
            case SUB_PARAM_AUXP1_MAP:
            case SUB_PARAM_AUXP2_MAP:                
                loadTrendData(subId);
                it.value()->trendDataInfo(_trendGraphInfo);
                break;
            default:
                it.value()->setVisible(false);
                continue;
            }
        }
        else if (trendGraphSetWidget.getTrendGroup() == TREND_GROUP_AG)
        {
            switch (subId)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_NIBP_MAP:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_ETN2O:
            case SUB_PARAM_ETAA1:
            case SUB_PARAM_ETAA2:
            case SUB_PARAM_ETO2:
            case SUB_PARAM_T1:
                loadTrendData(subId);
                it.value()->trendDataInfo(_trendGraphInfo);
                break;
            default:
                it.value()->setVisible(false);
                continue;
            }
        }

        it.value()->setFixedHeight(subWidgetHeight);
        it.value()->loadTrendSubWidgetInfo(info);
        it.value()->setTimeRange(_leftTime, _rightTime);
        it.value()->setVisible(true);
        _totalGraphNum ++;
    }

}

/**************************************************************************************************
 * 清除显示控件
 *************************************************************************************************/
void TrendWaveWidget::_clearLayout()
{
    int trendcount = _hLayoutTrend->count();
    for (int i = 0; i < trendcount; i ++)
    {
        QLayoutItem *item = _hLayoutTrend->takeAt(0);
        IWidget *widget = qobject_cast<IWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->deleteLater();
        }
    }
}

/**********************************************************************************************************************
 * 获取趋势数据
 **********************************************************************************************************************/
void TrendWaveWidget::_getTrendData()
{
    IStorageBackend *backend;
    backend = trendDataStorageManager.backend();
    int blockNum = backend->getBlockNR();
    QByteArray data;
    TrendDataSegment *dataSeg;
    TrendDataPackage *pack;
    qDeleteAll(_trendDataPack);
    _trendDataPack.clear();
    //TODO: low efficiency
    for (int i = 0; i < blockNum; i ++)
    {
        pack = new TrendDataPackage;
        data = backend->getBlockData((quint32)i);
        dataSeg = (TrendDataSegment*)data.data();
        pack->time = dataSeg->timestamp;
        for (int j = 0; j < dataSeg->trendValueNum; j ++)
        {
            pack->subparamValue[(SubParamID)dataSeg->values[j].subParamId] = dataSeg->values[j].value;
            if (!pack->alarmFlag && dataSeg->values[j].alarmFlag)
            {
                pack->alarmFlag = dataSeg->values[j].alarmFlag;
            }
        }
        _trendDataPack.append(pack);
    }
    int intervalNum = TrendDataSymbol::convertValue(_timeInterval)/TrendDataSymbol::convertValue(RESOLUTION_RATIO_5_SECOND);
    int displayDataNum = _trendDataPack.length() / intervalNum;
    _totalPage = (displayDataNum % GRAPH_POINT_NUMBER) ? ((displayDataNum / GRAPH_POINT_NUMBER) + 1) : (displayDataNum / GRAPH_POINT_NUMBER);
    _currentPage = 1;
}

void TrendWaveWidget::_initWaveSubWidget()
{
    TrendGraphType type;
    for (int i = 0; i < SUB_PARAM_NR; i ++)
    {
        switch (i)
        {
        case SUB_PARAM_HR_PR:
        case SUB_PARAM_SPO2:
        case SUB_PARAM_RR_BR:
        case SUB_PARAM_CVP_MAP:
        case SUB_PARAM_LAP_MAP:
        case SUB_PARAM_RAP_MAP:
        case SUB_PARAM_ICP_MAP:
            type = TREND_GRAPH_TYPE_NORMAL;
            break;
        case SUB_PARAM_NIBP_MAP:
            type = TREND_GRAPH_TYPE_NIBP;
            break;
        case SUB_PARAM_ETCO2:
        case SUB_PARAM_ETN2O:
        case SUB_PARAM_ETAA1:
        case SUB_PARAM_ETAA2:
        case SUB_PARAM_ETO2:
        case SUB_PARAM_T1:
            type = TREND_GRAPH_TYPE_AG_TEMP;
            break;
        case SUB_PARAM_ART_MAP:
        case SUB_PARAM_PA_MAP:
        case SUB_PARAM_AUXP1_MAP:
        case SUB_PARAM_AUXP2_MAP:
            type = TREND_GRAPH_TYPE_ART_IBP;
            break;
        default:
            continue;
        }

        TrendSubWaveWidget *subWidget = new TrendSubWaveWidget((SubParamID)i, type);
        subWidget->setVisible(true);
        subWidget->setParent(this);
        subWidget->setThemeColor(colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID((SubParamID)i))));
        subWidget->setFocusPolicy(Qt::NoFocus);
        _subWidgetMap.insert((SubParamID)i, subWidget);
        _hLayoutTrend->addWidget(subWidget);
    }
}

double TrendWaveWidget::_getCursorPos(unsigned t)
{
    double dpos = 0;
    dpos = (t - _leftTime) * (GRAPH_DATA_WIDTH / (_rightTime - _leftTime)) + (_waveRegionWidth - GRAPH_DATA_WIDTH) / 2;

    return dpos;
}
