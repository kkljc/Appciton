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
    _cursorPosition(GRAPH_POINT_NUMBER), _currentCursorTime(0),
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

    _getTrendData();

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
    _clearLayout();
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
    _cursorPosition = GRAPH_POINT_NUMBER;
    _clearLayout();
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
    _cursorPosition = GRAPH_POINT_NUMBER;
    _clearLayout();
    _trendLayout();

    update();

}

/**************************************************************************************************
 * 左移游标
 *************************************************************************************************/
void TrendWaveWidget::leftMoveCursor()
{
    if (_cursorPosition <= 0)
    {
        _cursorPosition = 0;
    }
    else if((GRAPH_POINT_NUMBER - _cursorPosition) == (_currentDisplayNum - 1) || (_currentDisplayNum == 0))
    {
        _cursorPosition = _cursorPosition;
    }
    else{
        _cursorPosition --;
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosition);
        }
    }

    update();
}

/**************************************************************************************************
 * 右移游标
 *************************************************************************************************/
void TrendWaveWidget::rightMoveCursor()
{
    if (_cursorPosition >= GRAPH_POINT_NUMBER)
    {
        _cursorPosition = GRAPH_POINT_NUMBER;
    }
    else{
        _cursorPosition ++;
    }

    int count = _hLayoutTrend->count();
    for (int i = 0; i < count; i++)
    {
        QLayoutItem *item = _hLayoutTrend->itemAt(i);
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
        if (widget != NULL)
        {
            widget->cursorMove(_cursorPosition);
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
    _cursorPosition = GRAPH_POINT_NUMBER;
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
        TrendSubWaveWidget *widget = dynamic_cast<TrendSubWaveWidget *>(item->widget());
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
    int intervalNum = TrendDataSymbol::convertValue(_timeInterval)/TrendDataSymbol::convertValue(RESOLUTION_RATIO_5_SECOND);
    _dataSize = _trendDataPack.length() / intervalNum - GRAPH_POINT_NUMBER * (_currentPage - 1);
    if (_dataSize > GRAPH_POINT_NUMBER + 1)
    {
        _dataSize = GRAPH_POINT_NUMBER + 1;
    }
    _dataBuf = new int[_dataSize];
    int j = 0;
    for (int i = _trendDataPack.length() - 1 - intervalNum * GRAPH_POINT_NUMBER * (_currentPage - 1); i  >= 0;
         i = i - intervalNum)
    {
        _dataBuf[j] = _trendDataPack.at(i)->subparamValue.value(subID, InvData());
        j ++;
        if (j == _dataSize)
        {
            break;
        }
    }
    _currentDisplayNum = j;
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

    unsigned t;
    int intervalNum = TrendDataSymbol::convertValue(_timeInterval)/TrendDataSymbol::convertValue(RESOLUTION_RATIO_5_SECOND);
    if (_trendDataPack.length() == 0)
    {
        t = _initTime;
    }
    else
    {
        t = _trendDataPack.at(_trendDataPack.length() - 1 - intervalNum * GRAPH_POINT_NUMBER * (_currentPage - 1))->time;
    }
    unsigned startT = t;
    QString tStr;
    unsigned onePixelTime;
    // 坐标刻度
    for (int i = GRAPH_DISPLAY_DATA_NUMBER; i >= 0; i --)
    {
        timeDate.getTime(t, tStr, true);
        barPainter.drawText(rectAdjust.topLeft().x() + (_waveRegionWidth - GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth - 30,
                            rectAdjust.topLeft().y() - 5, tStr);
        onePixelTime = TrendDataSymbol::convertValue(_timeInterval);
        t = t - onePixelTime * GRAPH_POINT_NUMBER/GRAPH_DISPLAY_DATA_NUMBER;
        barPainter.drawLine(rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y(), rectAdjust.topLeft().x() + (_waveRegionWidth -GRAPH_DATA_WIDTH)/2 + i * _oneFrameWidth,
                            rectAdjust.topLeft().y() + 5);
    }

    // 游标线
    barPainter.drawLine((_waveRegionWidth - GRAPH_DATA_WIDTH)/2 + _cursorPosition * DATA_INTERVAL_PIXEL, rectAdjust.topLeft().y(),
                        (_waveRegionWidth - GRAPH_DATA_WIDTH)/2 + _cursorPosition * DATA_INTERVAL_PIXEL, rectAdjust.bottomLeft().y());

    // 当前趋势记录的时间
    timeDate.getDate(startT - onePixelTime * (GRAPH_POINT_NUMBER - _cursorPosition), tStr);
    barPainter.drawText(_waveRegionWidth + 5, rectAdjust.topLeft().y() - 5, tStr);
    timeDate.getTime(startT - onePixelTime * (GRAPH_POINT_NUMBER - _cursorPosition), tStr, true);
    barPainter.drawText(_waveRegionWidth + 120, rectAdjust.topLeft().y() - 5, tStr);


    barPainter.setPen(QPen(Qt::white, 1, Qt::DotLine));
    barPainter.drawLine(_waveRegionWidth, 0, _waveRegionWidth, height());
}

void TrendWaveWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);
    _cursorPosition = GRAPH_POINT_NUMBER;
    changeTrendDisplay();
    update();
}

/**************************************************************************************************
 * 添加显示控件
 *************************************************************************************************/
void TrendWaveWidget::_trendLayout()
{
    int subWidgetHeight = (height() - 30)/_displayGraphNum;

    TrendSubWaveWidget *subWidget;
    _totalGraphNum = 0;    
    int *_dataBufSecond = NULL;
    int *_dataBufThird = NULL;
    for (int i = 0; i < SUB_PARAM_NR; i ++)
    {
        TrendSubWidgetInfo info;

        info.id = (SubParamID)i;
        info.xHead = (_waveRegionWidth -GRAPH_DATA_WIDTH)/2;
        info.xTail = (_waveRegionWidth + GRAPH_DATA_WIDTH)/2;
        info.yTop = subWidgetHeight/5;
        info.yBottom = subWidgetHeight/5*4;
        if (trendGraphSetWidget.getTrendGroup() == TREND_GROUP_RESP)
        {
            switch (i)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_RR_BR:
                info.type = TREND_GRAPH_TYPE_NORMAL;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)i);
                subWidget->trendData(_dataBuf, _dataSize);
                qFree(_dataBuf);
                _dataBuf = NULL;
                break;
            case SUB_PARAM_NIBP_MAP:
                info.type = TREND_GRAPH_TYPE_NIBP;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)(i - 1));
                _dataBufSecond = _dataBuf;
                loadTrendData((SubParamID)i);
                _dataBufThird = _dataBuf;
                loadTrendData((SubParamID)(i - 2));
                subWidget->trendData(_dataBuf, _dataBufSecond, _dataBufThird, _dataSize);
                qFree(_dataBuf);
                qFree(_dataBufSecond);
                qFree(_dataBufThird);
                _dataBuf = NULL;
                break;
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_T1:
                info.type = TREND_GRAPH_TYPE_AG_TEMP;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)(i + 1));
                _dataBufSecond = _dataBuf;
                loadTrendData((SubParamID)i);
                subWidget->trendData(_dataBuf, _dataBufSecond,  _dataSize);
                qFree(_dataBuf);
                qFree(_dataBufSecond);
                _dataBuf = NULL;
                break;
            default:
                continue;
            }
        }
        else if(trendGraphSetWidget.getTrendGroup() == TREND_GROUP_IBP)
        {
            SubParamID ibp1;
            SubParamID ibp2;
            ibpParam.getSubParamID(ibp1, ibp2);
            if (i != ibp1 && i != ibp2 &&
                    i != SUB_PARAM_HR_PR && i != SUB_PARAM_SPO2 &&
                    i != SUB_PARAM_NIBP_MAP && i != SUB_PARAM_T1)
            {
                continue;
            }
            switch (i)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_CVP_MAP:
            case SUB_PARAM_LAP_MAP:
            case SUB_PARAM_RAP_MAP:
            case SUB_PARAM_ICP_MAP:
                info.type = TREND_GRAPH_TYPE_NORMAL;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)i);
                subWidget->trendData(_dataBuf, _dataSize);
                qFree(_dataBuf);
                _dataBuf = NULL;
                break;
            case SUB_PARAM_NIBP_MAP:
                info.type = TREND_GRAPH_TYPE_NIBP;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)(i - 1));
                _dataBufSecond = _dataBuf;
                loadTrendData((SubParamID)(i - 2));
                _dataBufThird = _dataBuf;
                loadTrendData((SubParamID)i);
                subWidget->trendData(_dataBuf, _dataBufSecond, _dataBufThird, _dataSize);
                qFree(_dataBuf);
                qFree(_dataBufSecond);
                qFree(_dataBufThird);
                _dataBuf = NULL;
                break;
            case SUB_PARAM_T1:
                info.type = TREND_GRAPH_TYPE_AG_TEMP;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)(i + 1));
                _dataBufSecond = _dataBuf;
                loadTrendData((SubParamID)i);
                subWidget->trendData(_dataBuf, _dataBufSecond,  _dataSize);
                qFree(_dataBuf);
                qFree(_dataBufSecond);
                _dataBuf = NULL;
                break;
            case SUB_PARAM_ART_MAP:
            case SUB_PARAM_PA_MAP:
            case SUB_PARAM_AUXP1_MAP:
            case SUB_PARAM_AUXP2_MAP:
                info.type = TREND_GRAPH_TYPE_ART_IBP;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)(i - 1));
                _dataBufSecond = _dataBuf;
                loadTrendData((SubParamID)i);
                _dataBufThird = _dataBuf;
                loadTrendData((SubParamID)(i - 2));
                subWidget->trendData(_dataBuf, _dataBufSecond, _dataBufThird, _dataSize);
                qFree(_dataBuf);
                qFree(_dataBufSecond);
                qFree(_dataBufThird);
                _dataBuf = NULL;
                break;
            default:
                continue;
            }
        }
        else if (trendGraphSetWidget.getTrendGroup() == TREND_GROUP_AG)
        {
            switch (i)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
                info.type = TREND_GRAPH_TYPE_NORMAL;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)i);
                subWidget->trendData(_dataBuf, _dataSize);
                qFree(_dataBuf);
                _dataBuf = NULL;
                break;
            case SUB_PARAM_NIBP_MAP:
                info.type = TREND_GRAPH_TYPE_NIBP;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)(i + 1));
                _dataBufSecond = _dataBuf;
                loadTrendData((SubParamID)(i + 2));
                _dataBufThird = _dataBuf;
                loadTrendData((SubParamID)i);
                subWidget->trendData(_dataBuf, _dataBufSecond, _dataBufThird, _dataSize);
                qFree(_dataBuf);
                qFree(_dataBufSecond);
                qFree(_dataBufThird);
                _dataBuf = NULL;
                break;
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_ETN2O:
            case SUB_PARAM_ETAA1:
            case SUB_PARAM_ETAA2:
            case SUB_PARAM_ETO2:
            case SUB_PARAM_T1:
                info.type = TREND_GRAPH_TYPE_AG_TEMP;
                subWidget = new TrendSubWaveWidget(info);
                loadTrendData((SubParamID)(i + 1));
                _dataBufSecond = _dataBuf;
                loadTrendData((SubParamID)i);
                subWidget->trendData(_dataBuf, _dataBufSecond,  _dataSize);
                qFree(_dataBuf);
                qFree(_dataBufSecond);
                _dataBuf = NULL;
                break;
            default:
                continue;
            }
        }        
        subWidget->updateTrendGraph();
        subWidget->setFixedHeight(subWidgetHeight);
        subWidget->setVisible(true);
        subWidget->setParent(this);
        subWidget->setThemeColor(colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID((SubParamID)i))));
        _hLayoutTrend->addWidget(subWidget);
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
