#include "COTrendWidget.h"
#include "ColorManager.h"
#include "FontManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "TrendWidgetLabel.h"
#include "PublicMenuManager.h"
#include "WindowManager.h"
#include "COMenu.h"
#include <QDebug>

#define     INVALDATA     0xffff

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
COTrendWidget::COTrendWidget(const QString &trendName)
    : TrendWidget(trendName)
{
    _coStr = InvStr();
    _ciStr = InvStr();
    _tbStr = InvStr();

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    setPalette(palette);

    setName("C.O.");

    _coValue = new QLabel();
    _coValue->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    _coValue->setPalette(palette);
    _coValue->setText(InvStr());

    _ciName = new QLabel("C.I.");
    _ciName->setPalette(palette);
    _ciName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    _ciValue = new QLabel();
    _ciValue->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    _ciValue->setPalette(palette);
    _ciValue->setText(InvStr());

    _tbName = new QLabel("TB");
    _tbName->setPalette(palette);
    _tbName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    _tbValue = new QLabel();
    _tbValue->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    _tbValue->setPalette(palette);
    _tbValue->setText(InvStr());

    QHBoxLayout *ciLayout = new QHBoxLayout();
    ciLayout->addWidget(_ciName, 0, Qt::AlignCenter);
    ciLayout->addWidget(_ciValue, 0, Qt::AlignCenter);

    QHBoxLayout *tbLayout = new QHBoxLayout();
    tbLayout->addWidget(_tbName, 0, Qt::AlignCenter);
    tbLayout->addWidget(_tbValue, 0, Qt::AlignCenter);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(ciLayout);
    vLayout->addLayout(tbLayout);

    contentLayout->addWidget(_coValue);
    contentLayout->addLayout(vLayout);

    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
COTrendWidget::~COTrendWidget()
{

}

/**************************************************************************************************
 * display C.O. and C.I. data.。
 *************************************************************************************************/
void COTrendWidget::setMeasureResult(unsigned short coData, unsigned short ciData)
{
    if (coData == INVALDATA || ciData == INVALDATA)
    {
        _coStr = InvStr();
        _ciStr = InvStr();
    }
    else
    {
        unsigned short coInt = coData / 100;
        unsigned short coDec = coData % 100;
        unsigned short ciInt = ciData / 10;
        unsigned short ciDec = ciData % 10;
        _coStr = QString::number(coInt) + "." + QString::number(coDec);
        _ciStr = QString::number(ciInt) + "." + QString::number(ciDec);
    }

    _coValue->setText(_coStr);
    _ciValue->setText(_ciStr);

    return;
}

/**************************************************************************************************
 * display temp blood data。
 *************************************************************************************************/
void COTrendWidget::setTBData(unsigned short tbData)
{
    if (tbData == INVALDATA)
    {
        _tbStr = InvStr();
    }
    else
    {
        unsigned short tbInt = tbData / 100;
        unsigned short tbDec = tbData % 100;
        _tbStr = QString::number(tbInt) + "." + QString::number(tbDec);
    }

    _tbValue->setText(_tbStr);

    return;
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void COTrendWidget::setTextSize()
{
    QRect r;
    int h = ((height()-nameLabel->height()) / 3);
    int w = (width() - unitLabel->width());
    r.setSize(QSize(w, (h * 2)));

    int fontsize = fontManager.adjustNumFontSize(r,true,"2222");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _coValue->setFont(font);

    r.setSize(QSize(w, h));
    fontsize = fontManager.adjustNumFontSize(r,true,"2222");
    font = fontManager.numFont(fontsize);
    font.setWeight(QFont::Black);
    _ciName->setFont(font);
    _tbName->setFont(font);
    _ciValue->setFont(font);
    _tbValue->setFont(font);
}

/**************************************************************************************************
 * 趋势槽函数。
 *************************************************************************************************/
void COTrendWidget::_releaseHandle(IWidget *)
{
    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - publicMenuManager.width()) / 2;
    int y = r.y() + (r.height() - publicMenuManager.height());
    publicMenuManager.popup(&coMenu, x, y);
}