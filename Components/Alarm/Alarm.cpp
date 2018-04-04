#include "Alarm.h"
#include "BaseDefine.h"
#include "TimeDate.h"
#include "IConfig.h"
#include <QMap>
#include "ParamDataStorageManager.h"
#include "TrendCache.h"
#include "SummaryStorageManager.h"
#include "AlarmStateMachine.h"
#include "SystemManager.h"
#include "ECGParam.h"
#include "ParamManager.h"

#define ALARM_LIMIT_TIMES (3)   //超限3次后，发生报警
static int curSecondAlarmNum = 0; //record the number of alarms happend in the save seconds

struct AlarmTraceCtrl
{
    AlarmTraceCtrl()
    {
        Reset();
    }

    void Reset(void)
    {
        type = ALARM_TYPE_PHY;
        priority = ALARM_PRIO_LOW;
        alarmMessage = NULL;
        lastAlarmed = false;
        overHighLimit = false;
        normalTimesCount = 0;
        alarmTimesCount = 0;
        timestamp = 0;
        order = 0;      //record the order when alarms happened in the same seconds
    }

    AlarmType type;
    AlarmPriority priority;
    const char *alarmMessage;
    bool lastAlarmed;               //报警、拴锁报警
    bool overHighLimit;
    char order;
    unsigned normalTimesCount;
    unsigned alarmTimesCount;
    unsigned timestamp;
};
typedef QMap<QString, AlarmTraceCtrl> TraceMap;
static TraceMap _traceCtrl;

Alarm *Alarm::_selfObj = NULL;

/**************************************************************************************************
 * 功能：获取报警跟踪控制对象。
 * 参数：
 *      traceID：报警源ID。
 *************************************************************************************************/
static AlarmTraceCtrl &_getAlarmTraceCtrl(const QString traceID)
{
    TraceMap::iterator it = _traceCtrl.find(traceID);
    if (it != _traceCtrl.end())
    {
        return it.value();
    }

    _traceCtrl.insert(traceID, AlarmTraceCtrl());
    it = _traceCtrl.find(traceID);
    return it.value();
}

/**************************************************************************************************
 * 功能：获取报警源的ID，该ID用于跟踪报警状态。
 * 参数：
 *      ID: 报警源内部的ID号。
 *      traceID：返回报警源的跟踪ID。
 *************************************************************************************************/
void Alarm::_getAlarmID(AlarmParamIFace *alarmSource, int id, QString &traceID)
{
    //NOTE: this function is called too frequently, make a cache here
    typedef QPair<long, int> AlarmIDCacheKey;
    typedef QMap<AlarmIDCacheKey, QString> AlarmIDCacheType;
    static  AlarmIDCacheType alarmIdCache;


    AlarmIDCacheKey key((long)alarmSource, id);
    AlarmIDCacheType::iterator iter = alarmIdCache.find(key);
    if(iter == alarmIdCache.end())
    {
        traceID = alarmSource->getAlarmSourceName();
        traceID += QString::number(id);
        alarmIdCache.insert(key, traceID);
    }
    else
    {
        traceID = iter.value();
    }
}

/**************************************************************************************************
 * 功能：处理超限报警。
 *************************************************************************************************/
void Alarm::_handleLimitAlarm(AlarmLimitIFace *alarmSource, QList<ParamID> &alarmParam)
{
    bool isEnable = true;
    int completeResult = 0;
    int curValue = InvData();
    int n = alarmSource->getAlarmSourceNR();
    AlarmTraceCtrl *traceCtrl = NULL;
    QString traceID;

    for (int i = 0; i < n; i++)
    {
        // 获取该参数报警的跟踪ID，再取得跟踪对象。
        _getAlarmID(alarmSource, i, traceID);
        traceCtrl = &_getAlarmTraceCtrl(traceID);

        TrendCacheData data;
        trendCache.getTendData(_timestamp, data);
        curValue = data.value[alarmSource->getSubParamID(i)];

        isEnable = alarmSource->isAlarmEnable(i);
        completeResult = alarmSource->getCompare(curValue, i);

        // 报警关闭不处理超限报警
        if (_curAlarmStatus == ALARM_OFF)
        {
            traceCtrl->Reset();
            alarmSource->notifyAlarm(i, false);
            continue;
        }

        // 报警关闭
        if (!isEnable)
        {
            if (traceCtrl->lastAlarmed)
            {
                traceCtrl->normalTimesCount = ALARM_LIMIT_TIMES;
                if (alarmIndicator.latchAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage))
                {
                    alarmSource->notifyAlarm(i, true);
                    continue;
                }
            }

            traceCtrl->Reset();
            alarmSource->notifyAlarm(i, false);
            continue;
        }

        // 第一种情况：数据变为无效值或者报警恢复正常。
        if (curValue == InvData() || curValue == UnknownData() || (0 == completeResult))
        {
            if (traceCtrl->lastAlarmed)
            {
                if (traceCtrl->normalTimesCount >= ALARM_LIMIT_TIMES)
                {
                    if (alarmIndicator.latchAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage))
                    {
                        alarmSource->notifyAlarm(i, true);
                        continue;
                    }
                }
                else
                {
                    ++traceCtrl->normalTimesCount;
                    alarmSource->notifyAlarm(i, true);
                    continue;
                }
            }

            traceCtrl->Reset();
            alarmSource->notifyAlarm(i, false);
            continue;
        }

        if (!traceCtrl->lastAlarmed)
        {
            //超限报警持续一定的时间才发生报警
            ++traceCtrl->alarmTimesCount;
            if (traceCtrl->alarmTimesCount < ALARM_LIMIT_TIMES)
            {
                alarmSource->notifyAlarm(i, false);
                continue;
            }

            alarmSource->notifyAlarm(i, true);
            if (completeResult < 0) // 超低限。
            {
                traceCtrl->lastAlarmed = true;
                traceCtrl->overHighLimit = false;
                traceCtrl->type = ALARM_TYPE_PHY;
                traceCtrl->priority = alarmSource->getAlarmPriority(i);
                traceCtrl->alarmMessage = alarmSource->toString(i);
                traceCtrl->timestamp = _timestamp;
                traceCtrl->order = ++curSecondAlarmNum;
                alarmIndicator.addAlarmInfo(_timestamp, traceCtrl->type,
                        traceCtrl->priority, traceCtrl->alarmMessage);

                alarmParam.append(alarmSource->getParamID());
                summaryStorageManager.addPhyAlarm(_timestamp, alarmSource->getParamID(), i,
                        false, alarmSource->getWaveformID(i));
            }
            else if (completeResult > 0) // 超高限。
            {
                traceCtrl->lastAlarmed = true;
                traceCtrl->overHighLimit = true;
                traceCtrl->type = ALARM_TYPE_PHY;
                traceCtrl->priority = alarmSource->getAlarmPriority(i);
                traceCtrl->alarmMessage = alarmSource->toString(i);
                traceCtrl->timestamp = _timestamp;
                traceCtrl->order = ++curSecondAlarmNum;
                alarmIndicator.addAlarmInfo(_timestamp, traceCtrl->type,
                        traceCtrl->priority, traceCtrl->alarmMessage);

                alarmParam.append(alarmSource->getParamID());
                summaryStorageManager.addPhyAlarm(_timestamp, alarmSource->getParamID(), i,
                        false, alarmSource->getWaveformID(i));
            }
        }//栓锁的报警重新发生报警
        else
        {
            if (!alarmIndicator.checkAlarmIsExist(traceCtrl->type, traceCtrl->alarmMessage))
            {
                traceCtrl->Reset();
                ++traceCtrl->alarmTimesCount;
                alarmSource->notifyAlarm(i, false);
                continue;
            }

            alarmSource->notifyAlarm(i, true);
            if (traceCtrl->normalTimesCount >= ALARM_LIMIT_TIMES)
            {
                ++traceCtrl->alarmTimesCount;
                if (traceCtrl->alarmTimesCount < ALARM_LIMIT_TIMES)
                {
                    continue;
                }
                else
                {
                    alarmIndicator.addAlarmInfo(_timestamp, traceCtrl->type,
                            traceCtrl->priority, traceCtrl->alarmMessage);
                }
            }
        }

        // 发生报警或者重新从栓锁到报警
        traceCtrl->alarmTimesCount = 0;
        traceCtrl->normalTimesCount = 0;
    }
}

/**************************************************************************************************
 * 功能：处理OneShot报警。
 *************************************************************************************************/
void Alarm::_handleOneShotAlarm(AlarmOneShotIFace *alarmSource)
{
    AlarmTraceCtrl *traceCtrl = NULL;
    bool isAlarm = false;
    bool isEnable = false;
    bool isRemoveAfterLatch = false;
    AlarmType type = ALARM_TYPE_TECH;
    AlarmPriority priority = ALARM_PRIO_LOW;
    int n = alarmSource->getAlarmSourceNR();
    QString traceID;

    for (int i = 0; i < n; i++)
    {
        // 获取该参数报警的跟踪ID，再取得跟踪对象。
        _getAlarmID(alarmSource, i, traceID);
        traceCtrl = &_getAlarmTraceCtrl(traceID);

        isAlarm = alarmSource->isAlarmed(i);
        type = alarmSource->getAlarmType(i);
        priority = alarmSource->getAlarmPriority(i);
        isRemoveAfterLatch = alarmSource->isRemoveAfterLatch(i);

        // 报警关闭不处理生理报警报警
        if (_curAlarmStatus == ALARM_OFF && type != ALARM_TYPE_TECH)
        {
            alarmSource->notifyAlarm(i, false);
            traceCtrl->Reset();
            continue;
        }

        // 更新报警级别
        if (priority != traceCtrl->priority && NULL != traceCtrl->alarmMessage)
        {
            traceCtrl->priority = priority;
            alarmIndicator.updataAlarmPriority(traceCtrl->type, traceCtrl->alarmMessage, priority);
        }

        // 根据各自要求是否需要删除报警
        if (alarmSource->isNeedRemove(i))
        {
            if (traceCtrl->lastAlarmed)
            {
                alarmIndicator.delAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage);
            }

            alarmSource->notifyAlarm(i, false);
            traceCtrl->Reset();
            continue;
        }

        // 报警使能关闭
        isEnable = alarmSource->isAlarmEnable(i);
        if (!isEnable)
        {
            if (traceCtrl->lastAlarmed)
            {
                if (traceCtrl->type != ALARM_TYPE_TECH)
                {
                    if (alarmIndicator.latchAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage))
                    {
                        alarmSource->notifyAlarm(i, true);
                        continue;
                    }
                }
                else
                {
                    alarmIndicator.delAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage);
                }
            }

            traceCtrl->Reset();
            alarmSource->notifyAlarm(i, false);
            continue;
        }

        if (!isAlarm)
        {
            // 上次报警，现在恢复正常了。
            if (traceCtrl->lastAlarmed)
            {
                if (traceCtrl->type != ALARM_TYPE_TECH)
                {
                    if (!alarmIndicator.latchAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage))
                    {
                        alarmSource->notifyAlarm(i, false);
                        traceCtrl->Reset();
                    }
                    else
                    {
                        alarmSource->notifyAlarm(i, true);
                    }
                }
                else
                {
                    alarmIndicator.delAlarmInfo(traceCtrl->type, traceCtrl->alarmMessage);
                    traceCtrl->Reset();
                }
            }
            else
            {
                alarmSource->notifyAlarm(i, false);
            }

            continue;
        }
        else
        {
            if (traceCtrl->lastAlarmed)
            {
                // 生命报警从栓锁恢复到继续报警。
                if (traceCtrl->type != ALARM_TYPE_TECH)
                {
                    if (alarmIndicator.checkAlarmIsExist(traceCtrl->type, traceCtrl->alarmMessage))
                    {
                        alarmIndicator.updateLatchAlarmInfo(traceCtrl->alarmMessage, false);
                        alarmSource->notifyAlarm(i, true);
                        continue;
                    }
                    else
                    {
                        traceCtrl->Reset();
                    }
                }
                else
                {
                    continue;
                }
            }
        }

        // 更新跟踪信息。
        traceCtrl->lastAlarmed = isAlarm;
        traceCtrl->type = type;
        traceCtrl->alarmMessage = alarmSource->toString(i);
        traceCtrl->priority = priority;
        traceCtrl->timestamp = _timestamp;
        traceCtrl->order = ++curSecondAlarmNum;

        // 发布该报警。
        alarmIndicator.addAlarmInfo(_timestamp, traceCtrl->type,
                traceCtrl->priority, traceCtrl->alarmMessage, isRemoveAfterLatch);

        if (traceCtrl->type == ALARM_TYPE_LIFE)
        {
            alarmSource->notifyAlarm(i, true);
            summaryStorageManager.addOneshotAlarm(_timestamp);
        }
        else if (traceCtrl->type == ALARM_TYPE_PHY)
        {
            alarmSource->notifyAlarm(i, true);
            summaryStorageManager.addPhyAlarm(_timestamp, alarmSource->getParamID(), i, true, alarmSource->getWaveformID(i));
        }
    }
}

/**************************************************************************************************
 * 功能：处理报警。
 *************************************************************************************************/
void Alarm::_handleAlarm(void)
{
    // 处理生理超限报警
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    QList<ParamID> alarmParamID;
    QList<ParamID> paramID;
    paramID.clear();
    foreach(AlarmLimitIFace* source, limitAlarmSourceList)
    {
        alarmParamID.clear();
        _handleLimitAlarm(source, alarmParamID);
        if (!alarmParamID.isEmpty())
        {
            paramID.append(alarmParamID);
        }
    }

    // 合并同一时刻的所有生理超限报警
    if (-1 != paramID.indexOf(PARAM_NIBP))
    {
        //nibp alarm add after check sys, dia, map
        paramDataStorageManager.addAlarmData(_timestamp, PARAM_NIBP);
        paramID.removeAll(PARAM_NIBP);
    }

    if (!paramID.isEmpty())
    {
        paramDataStorageManager.addAlarmData(_timestamp, paramID.takeFirst());
    }

    // 处理生理、技术和生命报警。
    QList<AlarmOneShotIFace*> oneshotSourceList = _oneshotSources.values();
    foreach(AlarmOneShotIFace* source, oneshotSourceList)
    {
        _handleOneShotAlarm(source);
    }
}

/**************************************************************************************************
 * 功能：注册一个报警源。
 * 参数：
 *      alarmSource：报警源
 *************************************************************************************************/
void Alarm::addLimtSource(AlarmLimitIFace &alarmSource)
{
    _limitSources.insert(alarmSource.getParamID(), &alarmSource);
}

/**************************************************************************************************
 * 功能：注册一个报警源。
 * 参数：
 *      alarmSource：报警源
 *************************************************************************************************/
void Alarm::addOneShotSource(AlarmOneShotIFace &alarmSource)
{
    _oneshotSources.insert(alarmSource.getParamID(), &alarmSource);
}

/**************************************************************************************************
 * 功能：设置静音键状态。
 * 参数：
 *      isPressed： 是否为按下。
 *************************************************************************************************/
void Alarm::updateMuteKeyStatus(bool isPressed)
{
    if (isPressed)
    {
        alarmStateMachine.handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_PRESSED, 0, 0);
    }
    else
    {
        alarmStateMachine.handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_RELEASED, 0, 0);
    }
}

/**************************************************************************************************
 * 功能： 获取报警源参数的值。
 * 参数：
 *      data：带回数据；
 *      len：data的长度。
 *************************************************************************************************/
void Alarm::getAlarmSourceValue(short *data, int len)
{
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        int nr = source->getAlarmSourceNR();
        for (int j = 0; j < nr; j++)
        {
            int index = source->getSubParamID(j);
            if (index < len)
            {
                data[index] = source->getValue(j);
            }
        }
    }
}

/**************************************************************************************************
 * 功能： 获取报警源参数的报警状态,超限报警存在栓锁此接口仅能判断是否发生报警。
 * 参数：
 *      sourceName:报警源名称
 *      id：子参数的ID。
 *      return value: 0, No alarm ; 1, low limit alarm; 2, high limit alarm
 *************************************************************************************************/
char Alarm::getAlarmSourceStatus(const QString &sourceName, SubParamID id)
{
    unsigned char flag = 0;
    int i = 0;
    AlarmLimitIFace *source = NULL;
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    for(; i < limitAlarmSourceList.size(); i++)
    {
        if (sourceName == limitAlarmSourceList[i]->getAlarmSourceName())
        {
            source = limitAlarmSourceList[i];
            break;
        }
    }

    i = 0;
    if (NULL != source)
    {
        int n = source->getAlarmSourceNR();
        for (; i < n; ++i)
        {
            if (id == source->getSubParamID(i))
            {
                AlarmTraceCtrl *traceCtrl = NULL;
                QString traceID;

                _getAlarmID(source, i, traceID);
                traceCtrl = &_getAlarmTraceCtrl(traceID);

                if (traceCtrl->lastAlarmed)
                {
                    flag = traceCtrl->overHighLimit ? HighLimitAlarm : LowLimitAlarm;
                    break;
                }
            }
        }
    }

    return flag;
}

/**************************************************************************************************
 * 功能： 主运行，1妙运行一次。
 *************************************************************************************************/
void Alarm::mainRun(unsigned t)
{
    if (!_alarmStatusList.isEmpty())
    {
        _curAlarmStatus = _alarmStatusList.takeFirst();
    }

    curSecondAlarmNum = 0;
    _timestamp = t;
    _handleAlarm();
    alarmIndicator.publishAlarm(_curAlarmStatus);
}

/**************************************************************************************************
 * 功能： 添加报警状态。
 *************************************************************************************************/
void Alarm::addAlarmStatus(AlarmAudioStatus status)
{
    while (!_alarmStatusList.isEmpty())
    {
        int index = _alarmStatusList.indexOf(status);
        if (-1 != index)
        {
            _alarmStatusList.removeAt(index);
        }
        else
        {
            break;
        }
    }

    _alarmStatusList.append(status);
}

/***************************************************************************************************
 * getAlarmMessage : get alarm message from the alarm source
 **************************************************************************************************/
const char *Alarm::getAlarmMessage(ParamID paramId, int alarmType, bool isOneshotAlarm)
{
    if(isOneshotAlarm)
    {
        QList<AlarmOneShotIFace *> source = _oneshotSources.values(paramId);
        if(source.size() > 1 || source.size() == 0)
        {
            //multiple sources for paramid or no source;
            debug("Unable to get oneshot alarm message for param:%d\n", paramId);
            return NULL;
        }
        return source.first()->toString(alarmType);
    }
    else
    {
        QList<AlarmLimitIFace *> source = _limitSources.values(paramId);
        if(source.size() > 1 || source.size() == 0)
        {
            //multiple sources for paramid or no source;
            debug("Unable to get limit alarm message for param: %d\n", paramId);
            return NULL;
        }
        return source.first()->toString(alarmType);
    }
}

static bool alarmInfoLessThan(Alarm::AlarmInfo info1, Alarm::AlarmInfo info2)
{
    if(info1.timestamp == info2.timestamp)
    {
        return info1.order < info2.order;
    }
    else
    {
        return info1.timestamp < info2.timestamp;
    }
}

/***************************************************************************************************
 * getCurrentAlarmInfo : get the current alarm info
 **************************************************************************************************/
QList<Alarm::AlarmInfo> Alarm::getCurrentPhyAlarmInfo()
{
    QList<Alarm::AlarmInfo> alarmInfoList;
    Alarm::AlarmInfo almInfo;
    QList<AlarmLimitIFace*> limitAlarmSourceList = _limitSources.values();

    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        int  n = source->getAlarmSourceNR();
        for (int i = 0; i < n; i++ )
        {
            AlarmTraceCtrl *traceCtrl = NULL;
            QString traceID;

            _getAlarmID(source, i, traceID);
            traceCtrl = &_getAlarmTraceCtrl(traceID);
            if(traceCtrl->lastAlarmed)
            {
                almInfo.paramid = source->getParamID();
                almInfo.isOneshot = false;
                almInfo.alarmType = i;
                almInfo.timestamp = traceCtrl->timestamp;
                almInfo.order = traceCtrl->order;
                alarmInfoList.append(almInfo);
            }
        }
    }

    QList<AlarmOneShotIFace*> oneshotAlarmSouceList = _oneshotSources.values();
    foreach (AlarmOneShotIFace *source, oneshotAlarmSouceList)
    {
        int n = source->getAlarmSourceNR();
        for(int i =0; i < n; i++)
        {
            if(source->getAlarmType(i) == ALARM_TYPE_PHY
                    || source->getAlarmType(i) == ALARM_TYPE_LIFE)
            {
                AlarmTraceCtrl *traceCtrl = NULL;
                QString traceID;

                _getAlarmID(source, i, traceID);
                traceCtrl = &_getAlarmTraceCtrl(traceID);
                if(traceCtrl->lastAlarmed)
                {
                    almInfo.paramid = source->getParamID();
                    almInfo.isOneshot = true;
                    almInfo.alarmType = i;
                    almInfo.timestamp = traceCtrl->timestamp;
                    almInfo.order = traceCtrl->order;
                    alarmInfoList.append(almInfo);
                }
            }
        }
    }

    qSort(alarmInfoList.begin(), alarmInfoList.end(), alarmInfoLessThan);

    return alarmInfoList;

}


/***************************************************************************************************
 *  check specific oneshot alarm is active
 **************************************************************************************************/
bool Alarm::getOneShotAlarmStatus(AlarmOneShotIFace *iface, int alarmId)
{
    QList<AlarmOneShotIFace*> oneshotAlarmSouceList = _oneshotSources.values();
    if(oneshotAlarmSouceList.contains(iface))
    {
        AlarmTraceCtrl *traceCtrl = NULL;
        QString traceID;
        _getAlarmID(iface, alarmId, traceID);
        traceCtrl = &_getAlarmTraceCtrl(traceID);
        return traceCtrl->lastAlarmed;
    }
    else
    {
        return false;
    }
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
Alarm::Alarm()
{
    _alarmStatusList.clear();
    _curAlarmStatus = ALARM_AUDIO_NORMAL;
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
Alarm::~Alarm()
{
    // 删除报警源。
    QList<AlarmLimitIFace *> limitAlarmSourceList = _limitSources.values();
    foreach(AlarmLimitIFace *source, limitAlarmSourceList)
    {
        delete source;
    }

    _limitSources.clear();

    QList<AlarmOneShotIFace *> oneshotSourceList = _oneshotSources.values();
    foreach(AlarmOneShotIFace *source, oneshotSourceList)
    {
        delete source;
    }
    _oneshotSources.clear();
    _alarmStatusList.clear();
}