#include <QResizeEvent>
#include "IBPWaveWidget.h"
#include "IBPParam.h"
#include "IBPWaveRuler.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "WaveWidgetSelectMenu.h"
#include "WindowManager.h"
#include "ComboListPopup.h"
#include "Debug.h"
#include "IBPManualRuler.h"
#include "TimeDate.h"

/**************************************************************************************************
 * 添加波形数据。
 *************************************************************************************************/
void IBPWaveWidget::addWaveformData(short wave, int flag)
{
    addData(wave, flag);

    if (_isAutoRuler)
    {
        _autoRulerHandle(wave);
    }

}

/**************************************************************************************************
 * 设置标尺的标签值。
 *************************************************************************************************/
void IBPWaveWidget::setRuler(IBPRulerRange zoom)
{
    if (zoom != IBP_RULER_RANGE_AUTO && zoom != IBP_RULER_RANGE_MANUAL)
    {
        _zoom->setText((QString)IBPSymbol::convert(zoom) + "mmHg");
    }
    else if (zoom == IBP_RULER_RANGE_MANUAL)
    {
        _zoom->setText((QString)IBPSymbol::convert(zoom) + "(" + QString::number(_lowLimit) +
                       "-" + QString::number(_highLimit) + ")");
    }
    else if (zoom == IBP_RULER_RANGE_AUTO)
    {
        _zoom->setText((QString)IBPSymbol::convert(zoom) + "(" + QString::number(_lowLimit) +
                       "-" + QString::number(_highLimit) + ")");
    }
}

/**************************************************************************************************
 * 设置导联状态信息。
 *************************************************************************************************/
void IBPWaveWidget::setLeadSta(int info)
{
    if (info)
    {
        _leadSta->setText(trs("LeadOff"));
    }
    else
    {
        _leadSta->setText("");
    }
}

/**************************************************************************************************
 * 自动设置标尺弹出。
 *************************************************************************************************/
void IBPWaveWidget::displayManualRuler()
{
    ibpManualRuler.setWaveWidget(this);
    ibpManualRuler.setHighLowRuler(_highLimit, _lowLimit);
    ibpManualRuler.autoShow();
}

/**************************************************************************************************
 * 设置波形上下限。
 *************************************************************************************************/
void IBPWaveWidget::setLimit(int low, int high)
{
    _lowLimit = low;
    _highLimit = high;
    low = low * 10 + 1000;
    high = high * 10 + 1000;
    setValueRange(low, high);
}

/**************************************************************************************************
 * 设置标名。
 *************************************************************************************************/
void IBPWaveWidget::setEntitle(IBPPressureName entitle)
{
    _name->setText(IBPSymbol::convert(entitle));
    QString zoomStr = QString::number(ibpParam.limit[entitle].low) + "-" +
            QString::number(ibpParam.limit[entitle].high) + "mmHg";
    _zoom->setText(zoomStr);
    _entitle = entitle;
    setLimit(ibpParam.limit[getEntitle()].low, ibpParam.limit[getEntitle()].high);
}

/**************************************************************************************************
 * 获取标名。
 *************************************************************************************************/
IBPPressureName IBPWaveWidget::getEntitle()
{
    return _entitle;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPWaveWidget::IBPWaveWidget(WaveformID id, const QString &waveName, const IBPPressureName &entitle)
    : WaveWidget(waveName, IBPSymbol::convert(entitle)), _zoomList(NULL), _entitle(entitle)
{
    _autoRulerTracePeek = -10000;
    _autoRulerTraveVally = 10000;
    _autoRulerTime = 0;
    _isAutoRuler = false;

    setFocusPolicy(Qt::NoFocus);
    setID(id);
//    setValueRange(64, 192);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    setPalette(palette);

    int fontSize = fontManager.getFontSize(7);
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedSize(130, 30);
    _name->setText(getTitle());

    _ruler = new IBPWaveRuler(this);
    _ruler->setPalette(palette);
    _ruler->setFont(fontManager.textFont(fontManager.getFontSize(0)));
    addItem(_ruler);

    _zoom = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _zoom->setFont(fontManager.textFont(fontSize));
    _zoom->setFixedSize(120, 30);
    _zoom->setText((QString)IBPSymbol::convert(ibpParam.limit[entitle].index) + "mmHg");
    addItem(_zoom);
    connect(_zoom, SIGNAL(released(IWidget*)), this, SLOT(_IBPZoom(IWidget*)));

    _leadSta = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _leadSta->setFont(fontManager.textFont(fontSize));
    _leadSta->setFixedSize(120, 30);
    _leadSta->setText(trs("LeadOff"));
    _leadSta->setFocusPolicy(Qt::NoFocus);
    addItem(_leadSta);

    // 加载配置
//    _loadConfig();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPWaveWidget::~IBPWaveWidget()
{

}

void IBPWaveWidget::paintEvent(QPaintEvent *e)
{
    WaveWidget::paintEvent(e);
}

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void IBPWaveWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(margin(), margin());
    _zoom->move(margin() + _name->rect().width(), margin());
    _leadSta->move(margin() + _name->rect().width() + _zoom->rect().width(), margin());
    _ruler->resize(2, margin(), width() - 2, height() - margin() * 2);
    WaveWidget::resizeEvent(e);
}

void IBPWaveWidget::showEvent(QShowEvent *e)
{
    WaveWidget::showEvent(e);
}

void IBPWaveWidget::focusInEvent(QFocusEvent *)
{
    if (Qt::NoFocus != _name->focusPolicy())
    {
        _name->setFocus();
    }
}

void IBPWaveWidget::_IBPZoom(IWidget *widget)
{
    if (NULL == _zoomList)
    {
        _zoomList = new ComboListPopup(widget, POPUP_TYPE_USER, IBP_RULER_RANGE_NR, 0);
        for (int i = 0; i < IBP_RULER_RANGE_NR; i++)
        {
            _zoomList->addItemText(IBPSymbol::convert(IBPRulerRange(i)));
        }
        _zoomList->setItemDrawMark(false);
        _zoomList->setFont(fontManager.textFont(fontManager.getFontSize(1)));
        connect(_zoomList, SIGNAL(destroyed()), this, SLOT(_popupDestroyed()));
    }

    _zoomList->show();
}

/**************************************************************************************************
 * 弹出菜单销毁。
 *************************************************************************************************/
void IBPWaveWidget::_popupDestroyed()
{
    int index = _zoomList->getCurIndex();
    if (index == -1)
    {
        _zoomList = NULL;
        return;
    }

    setRuler((IBPRulerRange)index);
    if (index == IBP_RULER_RANGE_AUTO)
    {
        _isAutoRuler = true;
    }
    else if (index == IBP_RULER_RANGE_MANUAL)
    {
        _isAutoRuler = false;
        displayManualRuler();
    }
    else
    {
        _isAutoRuler = false;
        setLimit(ibpParam.rulerLimit[index].low, ibpParam.rulerLimit[index].high);
    }

    _zoomList = NULL;
}

/**************************************************************************************************
 * 自动标尺计算。
 * 参数:
 *      data： 波形数据。
 *************************************************************************************************/
void IBPWaveWidget::_autoRulerHandle(short data)
{
    _autoRulerTracePeek = (_autoRulerTracePeek < data) ? data : _autoRulerTracePeek;
    _autoRulerTraveVally = (_autoRulerTraveVally > data) ? data : _autoRulerTraveVally;

    unsigned t = timeDate.time();
    if (_autoRulerTime == 0)
    {
        _autoRulerTime = t;
        return;
    }

    if (abs(timeDate.difftime(_autoRulerTime, t)) < 6)
    {
        return;
    }

    _autoRulerTime = t;

    // 开始寻找最合适的标尺。
    int ruler = IBP_RULER_RANGE_FIRST;
    for (; ruler <= IBP_RULER_RANGE_THIRTEENTH; ruler ++)
    {
        if ((_autoRulerTracePeek <= ibpParam.rulerLimit[(IBPRulerRange)ruler].high * 10 + 1000))
        {
            if ((ruler == IBP_RULER_RANGE_SEVENTH) || (ruler == IBP_RULER_RANGE_EIGHTH))
            {
                if (_autoRulerTraveVally >= ibpParam.rulerLimit[(IBPRulerRange)ruler].low * 10 + 1000)
                {
                    break;
                }
                else if (ruler == IBP_RULER_RANGE_EIGHTH)
                {
                    ruler = IBP_RULER_RANGE_NINTH;
                    break;
                }
            }
            else
            {
                break;
            }
        }

    }

    // ruler为新的增益。
    if (ruler > IBP_RULER_RANGE_THIRTEENTH)
    {
        ruler = IBP_RULER_RANGE_THIRTEENTH;
    }

    _autoRulerTracePeek = -10000;
    _autoRulerTraveVally = 10000;

    setLimit(ibpParam.rulerLimit[(IBPRulerRange)ruler].low, ibpParam.rulerLimit[(IBPRulerRange)ruler].high);
    setRuler(IBP_RULER_RANGE_AUTO);
}