#include "TrendGraphWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "IButton.h"
#include "LanguageManager.h"
#include "TrendWaveWidget.h"
#include "TrendGraphSetWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "EventReviewWindow.h"
#include "OxyCRGEventWidget.h"
#include "TrendGraphPageGenerator.h"
#include "RecorderManager.h"

#define ITEM_HEIGHT             30
#define ITEM_WIDTH              100

TrendGraphWidget *TrendGraphWidget::_selfObj = NULL;

TrendGraphWidget::~TrendGraphWidget()
{
}

void TrendGraphWidget::waveNumberChange(int num)
{
    if (num > 0 && num <= 3)
    {
        _waveWidget->setWaveNumber(num);
    }
}

void TrendGraphWidget::timeIntervalChange(int timeInterval)
{
    _waveWidget->setTimeInterval((ResolutionRatio)timeInterval);
}

void TrendGraphWidget::updateTrendGraph()
{
    _waveWidget->updateTrendGraph();
}

void TrendGraphWidget::setSubWidgetRulerLimit(SubParamID id, int down, int up)
{
    _waveWidget->setRulerLimit(id, down, up);
}

void TrendGraphWidget::_leftMoveCoordinate()
{
    _waveWidget->leftMoveCoordinate();
}

void TrendGraphWidget::_rightMoveCoordinate()
{
    _waveWidget->rightMoveCoordinate();
}

void TrendGraphWidget::_leftMoveCursor()
{
    _waveWidget->leftMoveCursor();
}

void TrendGraphWidget::_rightMoveCursor()
{
    _waveWidget->rightMoveCursor();
}

void TrendGraphWidget::_leftMoveEvent()
{
//TODO
}

void TrendGraphWidget::_rightMoveEvent()
{
//TODO
}

void TrendGraphWidget::_trendGraphSetReleased()
{
    trendGraphSetWidget.autoShow();
}

void TrendGraphWidget::_upReleased()
{
    _waveWidget->pageUpParam();
}

void TrendGraphWidget::_downReleased()
{
    _waveWidget->pageDownParam();
}

void TrendGraphWidget::_printReleased()
{

    //QList<TrendGraphPageGenerator::TrendGraphInfo> infos;
    //TrendGraphPageGenerator::TrendGraphInfo info;

    //RecordPageGenerator *pageGenerator = new TrendGraphPageGenerator(infos, );

    //recorderManager.addPageGenerator(pageGenerator);
}

TrendGraphWidget::TrendGraphWidget() : _waveWidget(NULL), _moveCoordinate(NULL),
    _moveCursor(NULL), _moveEvent(NULL), _print(NULL), _set(NULL), _up(NULL),
    _down(NULL), _maxWidth(0), _maxHeight(0)
{
    setTitleBarText(trs("TrendGraph"));
    _maxWidth = windowManager.getPopMenuWidth();
    _maxHeight = windowManager.getPopMenuHeight();
    setFixedSize(_maxWidth, _maxHeight);

    int fontSize = fontManager.getFontSize(1);
    setFont(fontManager.textFont(fontSize));

    _waveWidget = new TrendWaveWidget();
    _waveWidget->setWidgetSize(_maxWidth - 10, _maxHeight - ITEM_HEIGHT * 3);
    _waveWidget-> changeTrendDisplay();

    _moveCoordinate = new IMoveButton(trs("MoveCoordinate"));
    _moveCoordinate->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveCoordinate->setFont(fontManager.textFont(fontSize));
    connect(_moveCoordinate, SIGNAL(leftMove()), this, SLOT(_leftMoveCoordinate()));
    connect(_moveCoordinate, SIGNAL(rightMove()), this, SLOT(_rightMoveCoordinate()));

    _moveCursor = new IMoveButton(trs("MoveCursor"));
    _moveCursor->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveCursor->setFont(fontManager.textFont(fontSize));
    connect(_moveCursor, SIGNAL(leftMove()), this, SLOT(_leftMoveCursor()));
    connect(_moveCursor, SIGNAL(rightMove()), this, SLOT(_rightMoveCursor()));

    _moveEvent = new IMoveButton(trs("MoveEvent"));
    _moveEvent->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _moveEvent->setFont(fontManager.textFont(fontSize));
    connect(_moveEvent, SIGNAL(leftMove()), this, SLOT(_leftMoveEvent()));
    connect(_moveEvent, SIGNAL(rightMove()), this, SLOT(_rightMoveEvent()));

    _print = new IButton(trs("Print"));
    _print->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _print->setFont(fontManager.textFont(fontSize));
    connect(_print, SIGNAL(realReleased()), this, SLOT(_printReleased()));

    _set = new IButton(trs("Set"));
    _set->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    _set->setFont(fontManager.textFont(fontSize));
    connect(_set, SIGNAL(realReleased()), this, SLOT(_trendGraphSetReleased()));

    _up = new IButton();
    _up->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _up->setPicture(QImage("/usr/local/nPM/icons/ArrowUp.png"));
    connect(_up, SIGNAL(realReleased()), this, SLOT(_upReleased()));

    _down = new IButton();
    _down->setFixedSize(ITEM_HEIGHT, ITEM_HEIGHT);
    _down->setPicture(QImage("/usr/local/nPM/icons/ArrowDown.png"));
    connect(_down, SIGNAL(realReleased()), this, SLOT(_downReleased()));

    QHBoxLayout *lineLayout = new QHBoxLayout();
    lineLayout->setMargin(0);
    lineLayout->setSpacing(2);
    lineLayout->addWidget(_moveCoordinate);
    lineLayout->addWidget(_moveCursor);
    lineLayout->addWidget(_moveEvent);
    lineLayout->addWidget(_print);
    lineLayout->addWidget(_set);
    lineLayout->addWidget(_up);
    lineLayout->addWidget(_down);

    contentLayout->addWidget(_waveWidget);
    contentLayout->addStretch();
    contentLayout->addLayout(lineLayout);
    contentLayout->addStretch();
}
