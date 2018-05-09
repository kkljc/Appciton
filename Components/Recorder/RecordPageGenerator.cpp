#include "RecordPageGenerator.h"
#include <QTimerEvent>
#include <QDateTime>
#include <qmath.h>
#include <QPainterPath>
#include <QPainter>
#include <QStringList>
#include "FontManager.h"
#include "LanguageManager.h"
#include "ParamInfo.h"
#include "SystemManager.h"
#include "ParamManager.h"

#define DEFAULT_PAGE_WIDTH 200
RecordPageGenerator::RecordPageGenerator(QObject *parent)
    :QObject(parent), _requestStop(false), _generate(true), _timerID(-1)
{
}

RecordPageGenerator::~RecordPageGenerator()
{
}

int RecordPageGenerator::type() const
{
    return Type;
}

void RecordPageGenerator::start(int interval)
{
    _timerID = startTimer(interval);
}

void RecordPageGenerator::stop()
{
    _requestStop = true;
}

void RecordPageGenerator::pageControl(bool pause)
{
    _generate = !pause;
}

QFont RecordPageGenerator::font() const
{
    return fontManager.recordFont(24);
}

RecordPage *RecordPageGenerator::createPage()
{
    RecordPage *p = new RecordPage(DEFAULT_PAGE_WIDTH);
#if 1
    //draw a sin wave
    qreal yOffset = p->height() / 2;
    qreal xOffset = 2 * 3.1415926 / DEFAULT_PAGE_WIDTH;
    QPainter painter(p);
    painter.setPen(Qt::white);
    static QPointF lastP(0, yOffset);
    QPointF p2;

    for(int i = 0; i< p->width(); i++)
    {
        p2.setX(i);
        p2.setY(yOffset - qSin(i * xOffset) * (p->height() / 2 - 2));
        painter.drawLine(lastP, p2);
        lastP = p2;
    }

    lastP.setX(0);
    lastP.setY(lastP.y() - 1);


#endif
    return p;
}

RecordPage *RecordPageGenerator::createTitlePage(const QString &title, const PatientInfo &patInfo, unsigned timestamp)
{
    QStringList infos;
    infos.append(QString("%1: %2").arg(trs("Name")).arg(patInfo.name));
    infos.append(QString("%1: %2").arg(trs("Gender")).arg(PatientSymbol::convert(patInfo.sex)));
    infos.append(QString("%1: %2").arg(trs("PatientType")).arg(PatientSymbol::convert(patInfo.type)));
    infos.append(QString("%1: %2").arg(trs("Blood")).arg(PatientSymbol::convert(patInfo.blood)));
    QString str;
    str = QString("%1: ").arg(trs("Age"));
    if(patInfo.age > 0)
    {
        str += QString::number(patInfo.age);
    }
    infos.append(str);

    str = QString("%1: ").arg(trs("Weight"));
    if(patInfo.weight)
    {
        str += QString("%1 %2").arg(QString::number(patInfo.weight)).arg(PatientSymbol::convert(patInfo.weightUnit));
    }
    infos.append(str);

    str = QString("%1: ").arg(trs("Height"));
    if(patInfo.height)
    {
        str += QString::number(patInfo.height);
    }
    infos.append(str);

    infos.append(QString("%1: ").arg(trs("ID")).arg(patInfo.id));

    //calculate the info text width
    int textWidth = 0;
    int w = 0;
    QFont font = fontManager.recordFont(24);
    foreach (QString infoStr, infos) {
        w = fontManager.textWidthInPixels(infoStr, font);
        if(w > textWidth)
        {
            textWidth = w;
        }
    }

    //title width
    w = fontManager.textWidthInPixels(title, font);
    if(w > textWidth)
    {
        textWidth =  w;
    }

    QDateTime dt = QDateTime::currentDateTime();
    if(timestamp)
    {
        dt = QDateTime::fromTime_t(timestamp);
    }

    QString timeStr = QString("%1: %2").arg(trs("RecordTime")).arg(dt.toString("yyyy-MM-dd HH:mm:ss"));

    //record time width
    w = fontManager.textWidthInPixels(timeStr, font);
    if(w > textWidth)
    {
        textWidth =  w;
    }

    int pageWidth = textWidth + font.pixelSize() * 3;
    int fontH = fontManager.textHeightInPixels(font);

    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    //we assume the page can hold all the rows
    QRect textRect(font.pixelSize(), fontH, textWidth, fontH);
    painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, title);

    //left one empty row
    textRect.translate(0, fontH/2);

    foreach (QString infoStr, infos) {
        textRect.translate(0, fontH);
        painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, infoStr);
    }

    //recording time in the bottom
    textRect.setTop(page->height() - fontH - fontH/2);
    textRect.setBottom(page->height() - fontH / 2);
    painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, timeStr);

    return page;
}


/**
 * @brief getSubParamNameHelper get the proper sub param name base on the module config
 * @param subParamId    sub param id
 * @param moduleConfig  the module config
 * @return sub param name
 */
static QString getSubParamNameHelper(SubParamID subParamId, short moduleConfig)
{
    if(subParamId == SUB_PARAM_HR_PR && !(moduleConfig & CONFIG_SPO2))
    {
        return trs(paramInfo.getSubParamName(SUB_DUP_PARAM_HR));
    }
    else if (subParamId == SUB_PARAM_RR_BR)
    {
        if ((moduleConfig & CONFIG_CO2) && (moduleConfig & CONFIG_RESP))
        {
            return trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR, true));
        }
        else if ((moduleConfig & CONFIG_CO2))
        {
            return trs(paramInfo.getSubParamName(SUB_DUP_PARAM_BR));
        }
        else
        {
            return trs(paramInfo.getSubParamName(SUB_DUP_PARAM_RR));
        }
    }
    else
    {
        return trs(paramInfo.getSubParamName(subParamId));
    }
}

/**
 * @brief contructNormalTrendStringItem contruct the trend string for params other than IBP or NIBP
 * @param subParamId sub param id
 * @param data the trend value
 * @param unit current display unit
 * @param defaultUnit default unit
 * @param co2Baro
 * @return trend string
 */
static QString contructNormalTrendStringItem(SubParamID subParamId, TrendDataType data,
                                        UnitType unit, UnitType defaultUnit, short co2Bro)
{
    //name
    QString trendString =  getSubParamNameHelper(subParamId, systemManager.getModuleConfig());
    trendString += "\t";

    //value
    int mul = paramInfo.getMultiOfSubParam(subParamId);
    if(data == InvData())
    {
        trendString += InvStr();
    }
    else
    {
        if(1 == mul)
        {
            trendString += Unit::convert(unit, defaultUnit, data, co2Bro);
        }
        else
        {
            trendString += Unit::convert(unit, defaultUnit, data * 1.0 / mul, co2Bro);
        }
    }
    trendString += "\t";

    //unit
    trendString += Unit::localeSymbol(unit);

    return trendString;
}

/**
 * @brief contructPressTrendStringItem contruct the press trend string for the IBP or NIBP
 * @param subParamId
 * @param data
 * @param unit
 * @param defaultUnit
 * @return
 */
static QString contructPressTrendStringItem(SubParamID subParamId, TrendDataType *data,
                                            UnitType unit, UnitType defaultUnit)
{
    QString trendString;
    Q_ASSERT(data != NULL);
    //name
    if (subParamId >= SUB_PARAM_ART_SYS && subParamId <= SUB_PARAM_AUXP2_SYS)
    {
        trendString = paramInfo.getIBPPressName(subParamId);
    }
    else if(subParamId >= SUB_PARAM_NIBP_SYS && subParamId <= SUB_PARAM_NIBP_MAP)
    {
        trendString = paramInfo.getParamName(PARAM_NIBP);
    }
    else
    {
        //should not get here
        return trendString;
    }
    trendString += "\t";


    //get value

    TrendDataType sys = InvData();
    TrendDataType dia = InvData();
    TrendDataType map = InvData();
    //have 3 press value by default
    int valueNum = 1;

    switch(subParamId)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_NIBP_SYS:
        valueNum = 3;
        sys = data[0];
        dia = data[1];
        map = data[2];
        break;
    default:
        map = data[0];
        break;
    }

    QString valueStr;
    if(valueNum == 1)
    {
        valueStr="(%1)";
        if(map == InvData())
        {
            valueStr = valueStr.arg(InvData());
        }
        else
        {
            int mul = paramInfo.getMultiOfSubParam(subParamId);
            if(mul == 1)
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, map));
            }
            else
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, map * 1.0 / mul));
            }
        }
    }
    else
    {
        valueStr= "%1/%2(%3)";
        if(sys == InvData())
        {
            valueStr = valueStr.arg(InvStr()).arg(InvStr()).arg(InvStr());
        }
        else
        {
            int mul = paramInfo.getMultiOfSubParam(subParamId);
            if(mul == 1)
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, sys))
                        .arg(Unit::convert(unit, defaultUnit, dia))
                        .arg(Unit::convert(unit, defaultUnit, map));
            }
            else
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, sys * 1.0 / mul))
                        .arg(Unit::convert(unit, defaultUnit, dia * 1.0 / mul))
                        .arg(Unit::convert(unit, defaultUnit, map * 1.0 / mul));
            }
        }
    }

    trendString += valueStr;
    trendString += "\t";


    //unit

    trendString += Unit::localeSymbol(unit);

    return trendString;
}

/**
 * @brief The TrendStringSegmentInfo struct
 *        use to handle the trend string info
 */
struct TrendStringSegmentInfo
{
    QString nameSegment;
    QString valueSegment;
    QString unitSegment;
    int nameSegmentWidth;
    int valueSegmentWidth;
    int unitSegmentWidth;
};

/**
 * @brief converToStringSegmets convert the trend string into segments
 * @param trendStringList input trend string list
 * @param strSegInfoList output string segments info list
 * @param font string display font
 */
static void converToStringSegmets(const QStringList &trendStringList, QList<TrendStringSegmentInfo>& strSegInfoList, const QFont &font)
{
    strSegInfoList.clear();
    foreach(QString str, trendStringList)
    {
        TrendStringSegmentInfo segInfo;
        segInfo.nameSegment = str.section('\t', 0, 0);
        segInfo.valueSegment = str.section('\t', 1, 1);
        segInfo.unitSegment = str.section('\t', 2, 2);
        segInfo.nameSegmentWidth = fontManager.textWidthInPixels(segInfo.nameSegment, font) + font.pixelSize();
        segInfo.valueSegmentWidth = fontManager.textWidthInPixels(segInfo.valueSegment, font) + font.pixelSize();
        segInfo.unitSegmentWidth = fontManager.textWidthInPixels(segInfo.unitSegment, font)+ font.pixelSize();
        strSegInfoList.append(segInfo);
    }
}

RecordPage *RecordPageGenerator::createTrendPage(const TrendDataPackage &trendData, bool showEventTime)
{
    QStringList trendStringList = getTrendStringList(trendData);

    QString timeStr;
    if(showEventTime)
    {
        QDateTime dt = QDateTime::fromTime_t(trendData.time);
        timeStr = QString("%1: %2").arg(trs("EventTime")).arg(dt.toString("yyyy-MM-dd HH:mm:ss"));
    }

    QFont font = fontManager.recordFont(24);

    int fontH = fontManager.textHeightInPixels(font);

    int avaliableTextHeight = RECORDER_PAGE_HEIGHT -  2 * fontH - fontH / 2;


    // if the contains too many lines, need to seperate into several group
    QVector<int> segmentWidths; //record the maximum widht or the group segments
    int avaliableLine = avaliableTextHeight / fontH;
    QList<TrendStringSegmentInfo> strSegInfoList;
    converToStringSegmets(trendStringList, strSegInfoList, font);
    int index = -1;
    int n = 0;
    foreach (TrendStringSegmentInfo segInfo, strSegInfoList) {
        if(n % avaliableLine == 0)
        {
            //each line contain 3 segments
            segmentWidths.append(0);
            segmentWidths.append(0);
            segmentWidths.append(0);
            index++;
        }
        n++;
        if(segInfo.nameSegmentWidth > segmentWidths[index * 3])
        {
            segmentWidths[index * 3] = segInfo.nameSegmentWidth;
        }
        if(segInfo.valueSegmentWidth> segmentWidths[index * 3 + 1])
        {
            segmentWidths[index * 3 + 1] = segInfo.valueSegmentWidth;
        }
        if(segInfo.unitSegmentWidth > segmentWidths[index * 3 + 2])
        {
            segmentWidths[index * 3 + 2] = segInfo.unitSegmentWidth;
        }
    }

    // calculate the page width of all the group line
    int pageWidth = 0;
    foreach(int maxW, segmentWidths)
    {
        pageWidth += maxW;
    }

    // check time string width, time string is drawn at the bottom
    if(showEventTime && pageWidth < fontManager.textWidthInPixels(timeStr))
    {
        pageWidth = fontManager.textWidthInPixels(timeStr);
    }

    // add the gap between group
    pageWidth += (segmentWidths.size() / 3) * font.pixelSize() + font.pixelSize() * 2;

    // Do the drawing stuff
    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    int xoffset = font.pixelSize();
    for(int i = 0; i<segmentWidths.size(); i+=3)
    {
        QRectF nameRect(xoffset, fontH, segmentWidths[i], fontH);
        xoffset += segmentWidths[i];
        QRectF valueRect(xoffset, fontH, segmentWidths[i+1], fontH);
        xoffset += segmentWidths[i+1];
        QRectF unitRect(xoffset, fontH, segmentWidths[i+2], fontH);
        xoffset += segmentWidths[i+2];
        TrendStringSegmentInfo seg;
        for(int lines = 0; lines < avaliableLine; lines++)
        {
            if(strSegInfoList.isEmpty())
            {
                break;
            }
            seg = strSegInfoList.takeFirst();

            painter.drawText(nameRect, seg.nameSegment, textOption);
            painter.drawText(valueRect, seg.valueSegment, textOption);
            painter.drawText(unitRect, seg.unitSegment, textOption);

            nameRect.translate(0, fontH);
            valueRect.translate(0, fontH);
            unitRect.translate(0, fontH);
        }
        xoffset += font.pixelSize();
    }


    if(showEventTime)
    {
        QRectF r;
        r.setLeft(font.pixelSize());
        r.setTop(page->height() - fontH - fontH / 2);
        r.setBottom(page->height() - fontH / 2);
        r.setHeight(fontH);
        r.setWidth(pageWidth);

        painter.drawText(r, timeStr, textOption);
    }

    return page;
}

static bool isPressSubParam(SubParamID SubParamID)
{
    switch (SubParamID) {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_NIBP_SYS:
    case SUB_PARAM_NIBP_DIA:
    case SUB_PARAM_NIBP_MAP:
        return true;
    default:
        break;
    }
    return false;
}

QStringList RecordPageGenerator::getTrendStringList(const TrendDataPackage &trendData)
{
    QStringList strList;
    ParamID paramid;
    SubParamID subparamID;
    for(int i = 0; i < SUB_PARAM_NR; i++)
    {
        subparamID = (SubParamID)i;
        if(!trendData.subparamValue.contains(subparamID))
        {
            continue;
        }

        paramid = paramInfo.getParamID(subparamID);
        if(!isPressSubParam(subparamID))
        {
            strList.append(contructNormalTrendStringItem(subparamID,
                                                         trendData.subparamValue[subparamID],
                                                         paramManager.getSubParamUnit(paramid, subparamID),
                                                        paramInfo.getUnitOfSubParam(subparamID),
                                                        trendData.co2Baro));
        }
        else
        {
            TrendDataType data[3] = {InvData()};
            bool handle = true;
            switch (subparamID) {
            case SUB_PARAM_NIBP_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_NIBP_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_NIBP_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_NIBP_MAP, InvData());
                break;
            case SUB_PARAM_ART_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_ART_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_ART_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_ART_MAP, InvData());
                break;
            case SUB_PARAM_PA_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_PA_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_PA_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_PA_MAP, InvData());
                break;
            case SUB_PARAM_CVP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_CVP_MAP, InvData());
                break;
            case SUB_PARAM_LAP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_LAP_MAP, InvData());
                break;
            case SUB_PARAM_RAP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_RAP_MAP, InvData());
                break;
            case SUB_PARAM_ICP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_ICP_MAP, InvData());
                break;
            case SUB_PARAM_AUXP1_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_AUXP1_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_AUXP1_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_AUXP1_MAP, InvData());
                break;
            case SUB_PARAM_AUXP2_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_AUXP2_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_AUXP2_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_AUXP2_MAP, InvData());
                break;
            default:
                handle = false;
                break;
            }

            if(!handle)
            {
               continue;
            }
            strList.append(contructPressTrendStringItem(subparamID,
                                                        data,
                                                        paramManager.getSubParamUnit(paramid, subparamID),
                                                        paramInfo.getUnitOfSubParam(subparamID)));
        }
    }
    return strList;
}

void RecordPageGenerator::timerEvent(QTimerEvent *ev)
{
    if(_timerID == ev->timerId())
    {
        if(_requestStop)
        {
            killTimer(_timerID);
            _timerID = -1;
            _requestStop = false;
            emit stop();
            return;
        }

        if(!_generate)
        {
            return;
        }

        RecordPage *page = createPage();
        if(page == NULL)
        {
            killTimer(_timerID);
            emit stop();
            return;
        }

        emit generatePage(page);
    }
}