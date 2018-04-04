#include <QEvent>
#include <QPainter>
#include "SoftKeyWidget.h"
#include "FontManager.h"

#define ICON_PATH "/usr/local/nPM/icons/"

/***************************************************************************************************
 * 功能：重绘事件。
 * 参数：
 *      e：事件。
 **************************************************************************************************/
void SoftkeyWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QRect r = rect();
    r.adjust(1, 1, -1, -1);

    if (_enableBorder)
    {
        if (_pressed)
        {
            painter.setBrush(_pressColor);
        }
        else
        {
            painter.setBrush(_releaseColor);
        }

        painter.setPen(QPen(_borderColor));
        painter.drawRoundedRect(r, 3, 3);
    }
    else
    {
        QRect rect = this->rect();
        rect.adjust(-1, -1, 1, 1);
        painter.setBrush(backgroundColor());
    }

    if (_isPixmapValid)
    {
        int height = r.height();
        int width = r.width();
        width = (width - height)/2;
        QRect PixmapR = r;
        PixmapR.adjust(width, 0, -width, 0);
//        PixmapR.setSize(QSize(height, height));
        painter.drawPixmap(PixmapR, _pixmap);
    }

    if (!_text.isEmpty())
    {
        QString txt = _text;
        int fontSize = _adjustFontSize(r, txt);
        painter.setPen(_color);
        painter.setFont(fontManager.textFont(fontSize, true));
        painter.drawText(r, Qt::AlignCenter | Qt::TextWordWrap, txt);
    }
    IWidget::paintEvent(e);
}

void SoftkeyWidget::resizeEvent(QResizeEvent */*e*/)
{

}

/***************************************************************************************************
 * 功能：键盘按下事件。
 * 参数：
 *      e：事件。
 **************************************************************************************************/
void SoftkeyWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat() && (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return))
    {
        e->ignore();
        return;
    }

    IWidget::keyPressEvent(e);
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        _pressed = true;
        update();
    }
}

/***************************************************************************************************
 * 功能：键盘弹起事件。
 * 参数：
 *      e：事件。
 **************************************************************************************************/
void SoftkeyWidget::keyReleaseEvent(QKeyEvent *e)
{
    if (e->isAutoRepeat() && (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return))
    {
        e->ignore();
        return;
    }

    IWidget::keyReleaseEvent(e);
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        _pressed = false;
        update();
    }
}

/***************************************************************************************************
 * 功能：鼠标按下事件。
 * 参数：
 *      e：事件。
 **************************************************************************************************/
void SoftkeyWidget::mousePressEvent(QMouseEvent *e)
{
    IWidget::mousePressEvent(e);
    _pressed = true;
    update();
}

/***************************************************************************************************
 * 功能：鼠标弹起事件。
 * 参数：
 *      e：事件。
 **************************************************************************************************/
void SoftkeyWidget::mouseReleaseEvent(QMouseEvent *e)
{
    IWidget::mouseReleaseEvent(e);
    _pressed = false;
    update();
}

/***************************************************************************************************
 * 功能：计算合适的字体大小。
 * 参数：
 *      r:控件大小。
 *      text:换行后的字串
 **************************************************************************************************/
int SoftkeyWidget::_adjustFontSize(const QRect &r, QString &txt)
{
    if (_text.isEmpty())
    {
        return 0;
    }

    if (r.width() < 4)
    {
        return 0;
    }

    int fontSize = fontManager.getFontSize(4);
    int width = r.width() - 6;
    QFont font = fontManager.textFont(fontSize, true);
    int txtLen = fontManager.textWidthInPixels(_text, font);
    if (txtLen <= width)
    {
        txt = _text;
        return fontSize;
    }

    QString text;
    int maxLen = 0;
    QStringList strList = _text.split(" ", QString::SkipEmptyParts);
    int count = strList.count();
    if (1 == count)
    {
        txt = _text;
        text = _text;
        maxLen = txtLen;
    }
    else if (2 == count)
    {
        txt = strList.at(0);
        txt += "\n";
        txt += strList.at(1);
        text = strList.at(0);
        maxLen = fontManager.textWidthInPixels(text, font);
        txtLen = fontManager.textWidthInPixels(strList.at(1), font);
        if (txtLen > maxLen)
        {
            text = strList.at(1);
            maxLen = txtLen;
        }
    }
    else
    {
        text = strList.at(0);
        text += " ";
        text += strList.at(1);
        txt = text;

        maxLen = fontManager.textWidthInPixels(text, font);

        QString tmpText;
        tmpText.clear();
        for (int i = 2; i < count - 1; ++i)
        {
            tmpText += strList.at(i);
            tmpText += " ";
        }
        tmpText += strList.at(count - 1);

        txtLen = fontManager.textWidthInPixels(tmpText, font);
        if (txtLen > maxLen)
        {
            maxLen = txtLen;
        }
        else
        {
            text = tmpText;
        }

        txt += "\n";
        txt += tmpText;
    }

    while (maxLen > width && fontSize > 0)
    {
        font = fontManager.textFont(fontSize, true);
        maxLen = fontManager.textWidthInPixels(text, font);
        --fontSize;
    }

    return fontSize;
}

/***************************************************************************************************
 * 功能：设置显示内容。
 * 参数：
 *      desc:信息结构体
 **************************************************************************************************/
void SoftkeyWidget::setContent(const KeyActionDesc &desc)
{
    QString path = ICON_PATH + desc.iconPath;

    if (!desc.iconPath.isEmpty())
    {
        _pixmap.load(path);
        _isPixmapValid = true;
    }
    else
    {
        _isPixmapValid = false;
    }

    if (!desc.focus)
    {
        setFocusPolicy(Qt::NoFocus);
    }
    else
    {
        setFocusPolicy(Qt::StrongFocus);
    }
    _text = desc.text;
    _color = desc.color;
    _enableBorder = desc.border;
    _pressColor = desc.pressColor;
    _releaseColor = desc.releaseColor;

    update();
}

/***************************************************************************************************
 * 外观按下效果控制。
 **************************************************************************************************/
void SoftkeyWidget::pressedEffect(void)
{
    _pressed = true;
    update();
}

/***************************************************************************************************
 * 外观默认效果控制。
 **************************************************************************************************/
void SoftkeyWidget::normalEffect(void)
{
    _pressed = false;
    update();
}

/***************************************************************************************************
 * 默认的背景色。
 **************************************************************************************************/
QColor SoftkeyWidget::backgroundColor(void)
{
    return QColor(20, 20, 20);
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
SoftkeyWidget::SoftkeyWidget(QWidget *parent) : IWidget("SoftkeyWidget", parent)
{
    _isPixmapValid = true;
    _pressed = false;
    //边框颜色
    _borderColor = QColor(100, 100, 100);
    _releaseColor = Qt::black;
    _pressColor = Qt::black;
}