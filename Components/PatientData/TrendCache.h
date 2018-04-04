﻿#pragma once
#include <QString>
#include "ParamInfo.h"
#include "PrintManager.h"
#include "UnitManager.h"
#include <QMutex>
#include <QList>

#define MAX_TREND_CACHE_NUM (120)//缓存的趋势数据条数

// 趋势缓存数据
struct TrendCacheData
{
    TrendCacheData()
    {
        lastNibpMeasureTime = 0;
        lastNibpMeasureSuccessTime = 0;
        co2baro = 0;
        // 默认为无效值
        for (int i = 0; i < SUB_PARAM_NR; ++i)
        {
            value[i] = InvData();
            valueIsDisplay[i] = false;
        }
    }

    const TrendCacheData &operator=(const TrendCacheData &data)
    {
        lastNibpMeasureTime = data.lastNibpMeasureTime;
        lastNibpMeasureSuccessTime = data.lastNibpMeasureSuccessTime;
        co2baro = data.co2baro;
        ::memcpy(value, data.value, sizeof(value));
        ::memcpy(valueIsDisplay, data.valueIsDisplay, sizeof(valueIsDisplay));
        return *this;
    }
    unsigned lastNibpMeasureTime;
    unsigned lastNibpMeasureSuccessTime;
    TrendDataType co2baro;
    TrendDataType value[SUB_PARAM_NR];
    bool valueIsDisplay[SUB_PARAM_NR];
};

struct TrendAlarmStatus
{
    TrendAlarmStatus()
    {
        ::memset(isAlarm, 0, SUB_PARAM_NR);
    }

    bool isAlarm[SUB_PARAM_NR];
};

struct TrendRecorder
{
    int toTimestamp;        /* record trend until this timestamp */
    void *obj;              /* object that the trend data record to */

    /* call when trend data ready*/
    void (*record) (unsigned timestamp, const TrendCacheData &trendData, const TrendAlarmStatus &alarmStatus, void *obj);
};

// 子参数ID、数值映射。
typedef QMap<unsigned, TrendCacheData> TrendCacheMap;
typedef QMap<unsigned, TrendAlarmStatus> TrendAlarmStatusCacheMap;

/**************************************************************************************************
 * Trend数据缓存
 *************************************************************************************************/
class TrendCache
{
public:
    static TrendCache &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TrendCache();
        }

        return *_selfObj;
    }
    static TrendCache *_selfObj;
    ~TrendCache();

public:

    // 收集趋势数据
    void collectTrendData(unsigned t, bool overwrite = false);
    void collectTrendAlarmStatus(unsigned t);

    // 获取趋势数据
    bool getTendData(unsigned t, TrendCacheData &data);
    bool getTrendAlarmStatus(unsigned t, TrendAlarmStatus &alarmStatus);

    unsigned getLastNibpMeasureTime() const { return _nibpMeasureTime;}
    unsigned getLastNibpSuccessMeasureTime() const { return _nibpMeasureSuccessTime;}

    /* register a recorder to record the trend data, it will deleted when timestamp expired */
    void registerTrendRecorder(const TrendRecorder &recorder);

    /* unregister a recorder, use the record object to find the recorder, return true if the recorder is remove */
    bool unregisterTrendRecorder(void *recordObj);

private:
    TrendCache();

private:
    TrendCacheMap _trendCacheMap;
    TrendAlarmStatusCacheMap _trendAlarmStatusCacheMap;
    QList<TrendRecorder> _recorders;
    unsigned _nibpMeasureTime;
    unsigned _nibpMeasureSuccessTime;
    QMutex _mutex;
};

#define trendCache (TrendCache::construction())
#define deleteTrendCache() (delete TrendCache::_selfObj)