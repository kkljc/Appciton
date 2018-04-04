#include "CO2Param.h"
#include "CO2WaveRuler.h"
#include "CO2WaveWidget.h"
#include <QPainter>

/**************************************************************************************************
 * 绘图函数。
 *************************************************************************************************/
void CO2WaveRuler::paintItem(QPainter &painter)
{
    if (_up == _low)
    {
        return;
    }

    int xLeft = x();
    int xRight = x() + width();
    int yUp = y();
    int yLow = y() + height() - 2;
    int yMid = (_mid - _low) * (yUp - yLow) / (_up - _low) + yLow;

    painter.setFont(font());
    painter.setPen(QPen(palette().windowText(), 1, Qt::DotLine));

    // 上标尺
    painter.drawLine(xLeft, yUp, xRight, yUp);

    // 中标尺
    painter.drawLine(xLeft, yMid, xRight, yMid);

    // 下标尺
    painter.drawLine(xLeft, yLow, xRight, yLow);
}

/**************************************************************************************************
 * 设置标尺的刻度。
 *************************************************************************************************/
void CO2WaveRuler::setRuler(double up, double mid, double low)
{
    _up = up;
    _mid = mid;
    _low = low;

    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CO2WaveRuler::CO2WaveRuler(CO2WaveWidget *wave) : WaveWidgetItem(wave, true)
{
    _up = 20.0;
    _mid = 10.0;
    _low = 0;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CO2WaveRuler::~CO2WaveRuler()
{
}
