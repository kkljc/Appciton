////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件中的文本标签
////////////////////////////////////////////////////////////////////////////////
#include <QPainter>
#include "OxyCRGWidgetLabel.h"
#include "OxyCRGWidget.h"

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 构造一个文本标签
//
// 参数:
// text: 标签文本字符串
// flags: 文本对齐标志, 接收如下标志用于控制字符串的绘制:
//		Qt::AlignLeft
//		Qt::AlignRight
//		Qt::AlignHCenter
//		Qt::AlignJustify
//		Qt::AlignTop
//		Qt::AlignBottom
//		Qt::AlignVCenter
//		Qt::AlignCenter
//		Qt::TextDontClip
//		Qt::TextSingleLine
//		Qt::TextExpandTabs
//		Qt::TextShowMnemonic
//		Qt::TextWordWrap
//		Qt::TextIncludeTrailingSpaces
// parent: 标签所归属的波形控件
// isBackground: true表示将标签融入背景位图, false表示标签动态绘制
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
OxyCRGWidgetLabel::OxyCRGWidgetLabel(const QString &text, int flags,
        OxyCRGWidget *wave) :
        IWidget("", wave),
        _text(text), _flags(flags), _isFixedWidth(false)
{
    if (NULL != wave)
    {
        _waveName = wave->name();
    }
    else
    {
        _waveName = "";
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构一个标签对象
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
OxyCRGWidgetLabel::~OxyCRGWidgetLabel()
{
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 设置标签文本
//
// 参数:
// text: 标签文本字符串
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void OxyCRGWidgetLabel::setText(const QString &text)
{
    if (text != _text)
    {
        _text = text;
        resizeToFitText();
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 设置标签对齐标志
//
// 参数:
// flags: 文本对齐标志, 接收如下标志用于控制字符串的绘制:
//		Qt::AlignLeft
//		Qt::AlignRight
//		Qt::AlignHCenter
//		Qt::AlignJustify
//		Qt::AlignTop
//		Qt::AlignBottom
//		Qt::AlignVCenter
//		Qt::AlignCenter
//		Qt::TextDontClip
//		Qt::TextSingleLine
//		Qt::TextExpandTabs
//		Qt::TextShowMnemonic
//		Qt::TextWordWrap
//		Qt::TextIncludeTrailingSpaces
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void OxyCRGWidgetLabel::setFlags(int flags)
{
    if (flags != _flags)
    {
        _flags = flags;
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 根据当前的字体和字号修改标签的尺寸
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void OxyCRGWidgetLabel::resizeToFitText()
{
    if (_isFixedWidth)
    {
        return;
    }

    QFontMetrics fm(font());
    QRect rect = fm.boundingRect(text());
    resize(rect.width(), rect.height());
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制函数
//
// 参数:
// painter: 绘图对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////

void OxyCRGWidgetLabel::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    QPainter painter(this);
    painter.setFont(font());
    painter.setPen(palette().color(QPalette::WindowText));
//    painter.setPen(Qt::red);

    QRect r = this->rect();
    r.adjust(10, 0, -10, 0);
    painter.drawText(r, _flags, _text);
}
