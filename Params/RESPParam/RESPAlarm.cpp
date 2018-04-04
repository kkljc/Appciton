#include "RESPAlarm.h"
#include "IConfig.h"
#include "RESPSymbol.h"
#include "PatientManager.h"
#include "ParamInfo.h"
#include "RESPParam.h"
#include "RESPDupParam.h"
#include "AlarmConfig.h"

RESPLimitAlarm *RESPLimitAlarm::_selfObj = NULL;

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString RESPLimitAlarm::getAlarmSourceName(void)
{
    QString source(paramInfo.getParamName(PARAM_RESP));
    return source;
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int RESPLimitAlarm::getAlarmSourceNR(void)
{
    return RESP_LIMIT_ALARM_NR / 2;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID RESPLimitAlarm::getWaveformID(int /*id*/)
{
    return WAVE_RESP;
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID RESPLimitAlarm::getSubParamID(int /*id*/)
{
    return SUB_PARAM_NONE;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority RESPLimitAlarm::getAlarmPriority(int /*id*/)
{
    return ALARM_PRIO_LOW;
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int RESPLimitAlarm::getValue(int /*id*/)
{
    return 0;
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool RESPLimitAlarm::isAlarmEnable(int /*id*/)
{
    return true;
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int RESPLimitAlarm::getUpper(int /*id*/)
{
    return 0;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int RESPLimitAlarm::getLower(int /*id*/)
{
    return 0;
}

/**************************************************************************************************
 * 生理参数报警比较。
 *************************************************************************************************/
int RESPLimitAlarm::getCompare(int /*value*/, int /*id*/)
{
    return 0;
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *RESPLimitAlarm::toString(int /*id*/)
{
    return NULL;
}


/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPLimitAlarm::RESPLimitAlarm()
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPLimitAlarm::~RESPLimitAlarm()
{

}

/**************************************************************************************************
 *************************************************************************************************/

RESPOneShotAlarm *RESPOneShotAlarm::_selfObj = NULL;

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString RESPOneShotAlarm::getAlarmSourceName(void)
{
    QString str(paramInfo.getParamName(PARAM_RESP));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int RESPOneShotAlarm::getAlarmSourceNR(void)
{
    return RESP_ONESHOT_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID RESPOneShotAlarm::getWaveformID(int id)
{
    if (id == RESP_ONESHOT_ALARM_APNEA)
    {
        return WAVE_RESP;
    }
    return WAVE_NONE;
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID RESPOneShotAlarm::getSubParamID(int /*id*/)
{
    return SUB_PARAM_NONE;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority RESPOneShotAlarm::getAlarmPriority(int id)
{
    switch (id)
    {
        case RESP_ONESHOT_ALARM_APNEA:
            return ALARM_PRIO_HIGH;

        default:
            return ALARM_PRIO_MED;
    }
}

/**************************************************************************************************
 * 该报警是否为生命报警，技术报警和生理/生命报警分开存放。
 *************************************************************************************************/
AlarmType RESPOneShotAlarm::getAlarmType(int id)
{
    if (id == RESP_ONESHOT_ALARM_APNEA)
    {
        return ALARM_TYPE_PHY;
    }
    else
    {
        return ALARM_TYPE_TECH;
    }
}

/**************************************************************************************************
 * notify alarm。
 *************************************************************************************************/
void RESPOneShotAlarm::notifyAlarm(int id, bool isAlarm)
{
    if (id == RESP_ONESHOT_ALARM_APNEA)
    {
        respDupParam.isAlarm(isAlarm, false);
    }
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *RESPOneShotAlarm::toString(int id)
{
    return RESPSymbol::convert((RESPOneShotType)id);
}

/**************************************************************************************************
 * 报警使能。
 *************************************************************************************************/
bool RESPOneShotAlarm::isAlarmEnable(int id)
{
    int v = 1;
    if (id == RESP_ONESHOT_ALARM_APNEA)
    {
        v = alarmConfig.isLimitAlarmEnable(SUB_PARAM_RR_BR);
    }

    return ((bool)v && respParam.isEnabled());
}

/**************************************************************************************************
 * remove alarm message after latch。
 *************************************************************************************************/
bool RESPOneShotAlarm::isRemoveAfterLatch(int /*id*/)
{
    return false;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPOneShotAlarm::RESPOneShotAlarm()
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPOneShotAlarm::~RESPOneShotAlarm()
{

}