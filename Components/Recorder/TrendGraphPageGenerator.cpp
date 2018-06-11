#include "TrendGraphPageGenerator.h"
#include "RecorderManager.h"
#include "TrendDataStorageManager.h"
#include "PatientManager.h"
#include <QPainter>
#include <QVector>
#include "FontManager.h"
#include "ParamInfo.h"
#include "Utility.h"
#include "AlarmConfig.h"


#define AXIS_X_SECTION_WIDTH (RECORDER_PIXEL_PER_MM * 16)
#define AXIS_Y_SECTION_HEIGHT (RECORDER_PIXEL_PER_MM * 8)

#define AXIS_X_SECTION_NUM 4
#define AXIS_Y_SECTION_NUM 2

#define AXIS_X_WIDTH (AXIS_X_SECTION_WIDTH * AXIS_X_SECTION_NUM)
#define AXIS_Y_HEIGH (AXIS_Y_SECTION_HEIGHT * AXIS_Y_SECTION_NUM + AXIS_Y_SECTION_HEIGHT / 4)

#define TICK_LENGTH (RECORDER_PIXEL_PER_MM)


class TrendGraphPageGeneratorPrivate
{
public:
    TrendGraphPageGeneratorPrivate()
        :curPageType(RecordPageGenerator::TitlePage),
        startTime(0),
        endTime(0),
        deltaT(0),
        curDrawnGraph(0),
        marginLeft(20 * RECORDER_PIXEL_PER_MM) //2 cm
    {}

    RecordPage *createGraphPage();

    /**
     * @brief getAxisInfo get the axis info
     * @note each page can be drawn only two graph at most, one on the top, the other on the bottom
     * @param page the record page
     * @param graphInfo graph info
     * @param onTop graph is on the top region or not
     */
    GraphAxisInfo getAxisInfo(const RecordPage *page, const TrendGraphInfo & graphInfo, bool onTop);

    RecordPage *drawGraphPage();

    RecordPageGenerator::PageType curPageType;
    unsigned startTime;
    unsigned endTime;
    unsigned deltaT;
    QList<TrendGraphInfo> trendGraphInfos;
    int curDrawnGraph;
    int marginLeft;
};


#define GRAPH_SPACING 24
GraphAxisInfo TrendGraphPageGeneratorPrivate::getAxisInfo(const RecordPage *page, const TrendGraphInfo &graphInfo, bool onTop)
{
    //draw two graph
    GraphAxisInfo axisInfo;
    SubParamID subParamID = graphInfo.subParamID;
    UnitType unit = graphInfo.unit;
    QString name;

    switch(subParamID)
    {
        case SUB_PARAM_NIBP_SYS:
        name = paramInfo.getParamName(PARAM_NIBP);
        break;
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_CVP_MAP:
        case SUB_PARAM_LAP_MAP:
        case SUB_PARAM_RAP_MAP:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP2_SYS:
        name = paramInfo.getIBPPressName(subParamID);
        break;
        default:
        name = paramInfo.getSubParamName(subParamID);
        break;
    }

    axisInfo.caption = QString("%1/%2")
            .arg(trs(name))
            .arg(trs(Unit::getSymbol(unit)));

    axisInfo.height =  AXIS_Y_HEIGH;
    axisInfo.width = AXIS_X_WIDTH;
    axisInfo.marginLeft = marginLeft;
    axisInfo.xSectionWidth = AXIS_X_SECTION_WIDTH;
    axisInfo.xSectionNum = AXIS_X_SECTION_NUM;
    axisInfo.validWidth = axisInfo.xSectionWidth * axisInfo.xSectionNum;
    axisInfo.ySectionHeight = AXIS_Y_SECTION_HEIGHT;
    axisInfo.ySectionNum = AXIS_Y_SECTION_NUM;
    axisInfo.validHeight = axisInfo.ySectionHeight * axisInfo.ySectionNum;
    axisInfo.tickLength = TICK_LENGTH;
    axisInfo.drawArrow = true;

    if(onTop)
    {
        axisInfo.origin = QPointF(marginLeft, page->height() / 2 - GRAPH_SPACING );
    }
    else
    {
        axisInfo.origin = QPointF(marginLeft, page->height() /2 + AXIS_Y_HEIGH);

        //calcuelate the x labels
        unsigned t = startTime;
        QStringList timeList;
        QList<int> dayList;
        for(int i=0; i < AXIS_X_SECTION_NUM && t < endTime; i++)
        {
            QDateTime dt = QDateTime::fromTime_t(t);
            timeList.append(dt.toString("hh:mm:ss"));
            dayList.append(dt.date().day());
            t += deltaT;
        }

        bool crossTwoDay = false;
        for(int i = 0; i < dayList.size() - 1; i++)
        {
            if(dayList.at(i) != dayList.at(i+1))
            {
                crossTwoDay = true;
                break;
            }
        }

        for(int i = 0; i< timeList.size(); i++)
        {
            if(crossTwoDay)
            {
                axisInfo.xLabels.append(QString("%1%2")
                                        .arg(timeList.at(i))
                                        .arg(dayList.at(i)));
            }
            else
            {
                axisInfo.xLabels.append(timeList.at(i));
            }
        }
    }

    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subParamID, unit);
    axisInfo.yLabels = QStringList()<<Util::convertToString(graphInfo.scale.min, config.scale)
                                   <<QString()
                                  <<Util::convertToString(graphInfo.scale.max, config.scale);
    return axisInfo;
}

RecordPage *TrendGraphPageGeneratorPrivate::drawGraphPage()
{
    if(curDrawnGraph == trendGraphInfos.size())
    {
        //all pages are drawn
        return NULL;
    }

    int pageWidth = marginLeft + AXIS_X_SECTION_NUM * AXIS_X_SECTION_WIDTH;
    RecordPage *page =  new RecordPage(pageWidth);
    QPainter painter(page);

    QFont font = fontManager.recordFont(24);
    painter.setFont(font);
    if(curDrawnGraph + 2 <= trendGraphInfos.size())
    {
        //draw two graph
        GraphAxisInfo axisInfo = getAxisInfo(page, trendGraphInfos.at(curDrawnGraph), true);

        //draw the axis
        RecordPageGenerator::drawGraphAxis(&painter, axisInfo);

        //draw graph
        RecordPageGenerator::drawTrendGraph(&painter, axisInfo, trendGraphInfos.at(curDrawnGraph));

        curDrawnGraph++;

        axisInfo = getAxisInfo(page, trendGraphInfos.at(curDrawnGraph), false);
        RecordPageGenerator::drawGraphAxis(&painter, axisInfo);
        RecordPageGenerator::drawTrendGraph(&painter, axisInfo, trendGraphInfos.at(curDrawnGraph));
        curDrawnGraph++;
    }
    else
    {
        //draw only one graph
        GraphAxisInfo axisInfo = getAxisInfo(page, trendGraphInfos.at(curDrawnGraph), false);
        RecordPageGenerator::drawGraphAxis(&painter, axisInfo);
        RecordPageGenerator::drawTrendGraph(&painter, axisInfo, trendGraphInfos.at(curDrawnGraph));
        curDrawnGraph++;
    }

    return page;
}

TrendGraphPageGenerator::TrendGraphPageGenerator(const QList<TrendGraphInfo> &trendInfos, QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new TrendGraphPageGeneratorPrivate)
{
    if(trendInfos.size() > 0)
    {
        d_ptr->startTime = trendInfos.first().startTime;
        d_ptr->endTime = trendInfos.first().endTime;
    }
    d_ptr->trendGraphInfos = trendInfos;
    d_ptr->deltaT = (d_ptr->endTime - d_ptr->startTime) /  AXIS_X_SECTION_NUM;
}

TrendGraphPageGenerator::~TrendGraphPageGenerator()
{

}

int TrendGraphPageGenerator::type() const
{
    return Type;
}

RecordPage *TrendGraphPageGenerator::createPage()
{
    switch(d_ptr->curPageType)
    {
    case TitlePage:
        // BUG: patient info of the event might not be the current session patient
        d_ptr->curPageType = TrendGraphPage;
        return createTitlePage(QString(trs("GraphTrendRecording")), patientManager.getPatientInfo());

    case TrendGraphPage:
        if(d_ptr->curDrawnGraph < d_ptr->trendGraphInfos.size())
        {
            return d_ptr->drawGraphPage();
        }

        //fall through
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}
