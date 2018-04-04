#include "SPO2TrendWidget.h"
#include "ParamManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "SPO2Menu.h"
#include "PublicMenuManager.h"
#include "WindowManager.h"
#include "SPO2Param.h"
#include "TrendWidgetLabel.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void SPO2TrendWidget::_releaseHandle(IWidget *)
{
    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
    int y = r.y() + (r.height() - publicMenuManager.height());

    publicMenuManager.popup(&spo2Menu, x, y);
}

/**************************************************************************************************
 * 设置SPO2的值。
 *************************************************************************************************/
void SPO2TrendWidget::setSPO2Value(short spo2)
{
//    debug("------------spo2 = %d",spo2);
    if (spo2 >= 0)
    {
        _spo2String = QString::number(spo2);
    }
    else if (spo2 == UnknownData())
    {dbg;
        _spo2String = UnknownStr();
    }
    else
    {
        _spo2String = InvStr();
    }
}

/**************************************************************************************************
 * 设置bar图的值。
 *************************************************************************************************/
void SPO2TrendWidget::setBarValue(short bar)
{
    _spo2Bar->setValue(bar);
}

/**************************************************************************************************
 * 显示搜索脉搏的提示信息。
 *************************************************************************************************/
void SPO2TrendWidget::setSearchForPulse(bool isSearching)
{
    if (isSearching)
    {
//        setInfo(trs("SPO2SearchPulse"));
    }
    else
    {
//        setInfo(" ");
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void SPO2TrendWidget::isAlarm(bool flag)
{
    _isAlarm = flag;

    updateAlarm(flag);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void SPO2TrendWidget::showValue(void)
{
    QPalette p = palette();
    if (_isAlarm)
    {
        if (p.window().color() != Qt::white)
        {
            p.setColor(QPalette::Window, Qt::white);
            p.setColor(QPalette::WindowText, Qt::red);
            setPalette(p);
            _spo2Bar->setPalette(p);
        }

        p = _spo2Value->palette();
        if (p.windowText().color() != Qt::red)
        {
            p.setColor(QPalette::WindowText, Qt::red);
            _spo2Value->setPalette(p);
        }
    }
    else
    {
        if (p.window().color() != Qt::black)
        {
            p = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
            setPalette(p);
            _spo2Value->setPalette(p);
            _spo2Bar->setPalette(p);
        }
    }

    _spo2Value->setText(_spo2String);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void SPO2TrendWidget::setTextSize()
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

    _spo2Value->setFont(font);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SPO2TrendWidget::SPO2TrendWidget() : TrendWidget("SPO2TrendWidget")
{
    _isAlarm = false;
    _spo2String = InvStr();
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    setPalette(palette);
    setName(trs(paramInfo.getParamName(PARAM_SPO2)));
    setUnit(Unit::localeSymbol(UNIT_PERCENT));

    // 血氧值。
    _spo2Value = new QLabel();
    _spo2Value->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _spo2Value->setPalette(palette);
    _spo2Value->setText(InvStr());

    // 棒图。
    _spo2Bar = new SPO2BarWidget(0, 15);
    _spo2Bar->setFixedWidth(10);
    mLayout->addWidget(_spo2Bar);

    // 布局。
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addStretch(1);
    mainLayout->addWidget(_spo2Value);
    mainLayout->addStretch(1);

    contentLayout->addStretch(1);
    contentLayout->addLayout(mainLayout);
    contentLayout->addStretch(1);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SPO2TrendWidget::~SPO2TrendWidget()
{

}