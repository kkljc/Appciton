#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QTimer>
#include "ECGTrendWidget.h"
#include "ParamManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "ECGDupParam.h"
#include "Debug.h"
#include "ECGMenu.h"
#include "PublicMenuManager.h"
#include "TrendWidgetLabel.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "ECGDupParam.h"
#include "qpainter.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void ECGTrendWidget::_releaseHandle(IWidget *)
{
    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
    int y = r.y() + (r.height() - publicMenuManager.height());

    publicMenuManager.popup(&ecgMenu, x, y);
}

/**************************************************************************************************
 * 定时时间到。
 *************************************************************************************************/
void ECGTrendWidget::_timeOut()
{
    _timer->stop();
    _hrBeatIcon->setPixmap(QPixmap());
}

/**************************************************************************************************
 * 设置HR的值。
 *************************************************************************************************/
void ECGTrendWidget::setHRValue(short hr, bool isHR)
{
    if (isHR)
    {
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_HR)));

        if (windowManager.getUFaceType() == UFACE_MONITOR_12LEAD)
        {
            int index = ecgParam.getCalcLead();
            ECGLeadNameConvention convention = ecgParam.getLeadConvention();
            if (index == ECG_LEAD_AVR)
            {
                setCalcLeadName(ECGSymbol::convert((ECGLead)index, convention, true, false));
            }
            else
            {
                bool isCabrela = (ecgParam.get12LDisplayFormat() == DISPLAY_12LEAD_CABRELA);
                setCalcLeadName(ECGSymbol::convert((ECGLead)index, convention, true, isCabrela));
            }
        }
    }
    else
    {
        if (_hrBeatIcon->isVisible())
        {
            _hrBeatIcon->setPixmap(QPixmap());
        }
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_PR)));

        if (windowManager.getUFaceType() == UFACE_MONITOR_12LEAD)
        {
            setCalcLeadName("");
        }
    }

    if (hr >= 0)
    {
        _hrString = QString::number(hr);
    }
    else if (hr == UnknownData())
    {
        _hrString = UnknownStr();
    }
    else
    {
        _hrString = InvStr();
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void ECGTrendWidget::isAlarm(bool isAlarm)
{
    _isAlarm = isAlarm;

    updateAlarm(isAlarm);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void ECGTrendWidget::showValue(void)
{
    QPalette p = palette();
    if (_isAlarm)
    {
        if (p.window().color() != Qt::white)
        {
            p.setColor(QPalette::Window, Qt::white);
            p.setColor(QPalette::WindowText, Qt::red);
            setPalette(p);
        }

        p = _hrValue->palette();
        if (p.windowText().color() != Qt::red)
        {
            p.setColor(QPalette::WindowText, Qt::red);
            _hrValue->setPalette(p);
        }
    }
    else
    {
        if (p.window().color() != Qt::black)
        {
            p = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
            setPalette(p);
            _hrValue->setPalette(p);
        }
    }

    _hrValue->setText(_hrString);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void ECGTrendWidget::showEvent(QShowEvent *e)
{
    TrendWidget::showEvent(e);

    if (windowManager.getUFaceType() != UFACE_MONITOR_12LEAD)
    {
        setCalcLeadName("");
    }
    else
    {
        if (ecgDupParam.isHRValid())
        {
            int index = ecgParam.getCalcLead();
            ECGLeadNameConvention convention = ecgParam.getLeadConvention();
            if (index == ECG_LEAD_AVR)
            {
                setCalcLeadName(ECGSymbol::convert((ECGLead)index, convention, true, false));
            }
            else
            {
                bool isCabrela = (ecgParam.get12LDisplayFormat() == DISPLAY_12LEAD_CABRELA);
                setCalcLeadName(ECGSymbol::convert((ECGLead)index, convention, true, isCabrela));
            }
        }
        else
        {
            setCalcLeadName("");
        }
    }
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void ECGTrendWidget::setTextSize(void)
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    // 字体。
//    int fontsize = fontManager.adjustNumFontSizeXML(r);
//    fontsize = fontManager.getFontSize(fontsize);
    int fontsize = fontManager.adjustNumFontSize(r,true);
    QFont font = fontManager.numFont(fontsize, true);
//    font.setStretch(105); // 横向放大。
    font.setWeight(QFont::Black);

    _hrValue->setFont(font);
}

/**************************************************************************************************
 * 闪烁心跳图标。
 *************************************************************************************************/
void ECGTrendWidget::blinkBeatPixmap()
{
    _hrBeatIcon->setPixmap(beatPixmap);
    _timer->start();
}

/**************************************************************************************************
 * 设置计算导联字串。
 *************************************************************************************************/
void ECGTrendWidget::setTrendWidgetCalcName(ECGLead calLead)
{
    if (windowManager.getUFaceType() != UFACE_MONITOR_12LEAD)
    {
        setCalcLeadName("");
    }
    else
    {
        ECGLeadNameConvention convention = ecgParam.getLeadConvention();
        if (calLead == ECG_LEAD_AVR)
        {
            setCalcLeadName(ECGSymbol::convert(calLead, convention, true, false));
        }
        else
        {
            bool isCabrela = (ecgParam.get12LDisplayFormat() == DISPLAY_12LEAD_CABRELA);
            setCalcLeadName(ECGSymbol::convert(calLead, convention, true, isCabrela));
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGTrendWidget::ECGTrendWidget() : TrendWidget("ECGTrendWidget")
{
    _isAlarm = false;
    _hrString = InvStr();

    // 设置标题栏的相关信息。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);
    setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_HR)));
    setUnit(Unit::getSymbol(UNIT_BPM));

    // 开始布局。
    _hrBeatIcon = new QLabel();
    _hrBeatIcon->setFixedSize(24, 24);
    QImage destImage("/usr/local/nPM/icons/beat.png");
    QImage srcImage(destImage.width(), destImage.width(), QImage::Format_ARGB32);
    srcImage.fill(palette.foreground().color());
    QPainter painter(&srcImage);
    QPoint point(0, 0);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.drawImage(point, destImage);
    painter.end();
    beatPixmap =  QPixmap::fromImage(srcImage);
    _hrBeatIcon->setPixmap(QPixmap());

    _hrValue = new QLabel();
    _hrValue->setPalette(palette);
    _hrValue->setAlignment(Qt::AlignCenter);
    _hrValue->setText(InvStr());

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(4);
    hlayout->setSpacing(0);
    hlayout->addWidget(_hrValue);
    hlayout->addWidget(_hrBeatIcon);

    contentLayout->addStretch(1);
    contentLayout->addLayout(hlayout);
    contentLayout->addStretch(1);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

    _isAlarm = false;

    _timer = new QTimer();
    _timer->setInterval(190);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGTrendWidget::~ECGTrendWidget()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}