#include "AGAlarm.h"
#include "ParamInfo.h"
#include "AlarmConfig.h"
#include "AGParam.h"

AGLimitAlarm *AGLimitAlarm::_selfObj = NULL;

/**************************************************************************************************
 * alarm source name.
 *************************************************************************************************/
QString AGLimitAlarm::getAlarmSourceName()
{
    return paramInfo.getParamName(PARAM_AG);
}

/**************************************************************************************************
 * alarm source number.
 *************************************************************************************************/
int AGLimitAlarm::getAlarmSourceNR()
{
    return AG_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * alarm id corresponding to waveform ID
 *************************************************************************************************/
WaveformID AGLimitAlarm::getWaveformID(int id)
{
    switch (id)
    {
    case AG_LIMIT_ALARM_ETN2O_LOW:
    case AG_LIMIT_ALARM_ETN2O_HIGH:
    case AG_LIMIT_ALARM_FIN2O_LOW:
    case AG_LIMIT_ALARM_FIN2O_HIGH:
        return WAVE_N2O;
    case AG_LIMIT_ALARM_ETAA1_LOW:
    case AG_LIMIT_ALARM_ETAA1_HIGH:
    case AG_LIMIT_ALARM_FIAA1_LOW:
    case AG_LIMIT_ALARM_FIAA1_HIGH:
        return WAVE_AA1;
    case AG_LIMIT_ALARM_ETAA2_LOW:
    case AG_LIMIT_ALARM_ETAA2_HIGH:
    case AG_LIMIT_ALARM_FIAA2_LOW:
    case AG_LIMIT_ALARM_FIAA2_HIGH:
        return WAVE_AA2;
    case AG_LIMIT_ALARM_ETO2_LOW:
    case AG_LIMIT_ALARM_ETO2_HIGH:
    case AG_LIMIT_ALARM_FIO2_LOW:
    case AG_LIMIT_ALARM_FIO2_HIGH:
        return WAVE_O2;
    default:
        return WAVE_NONE;
    }
}

/**************************************************************************************************
 * alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID AGLimitAlarm::getSubParamID(int id)
{
    switch (id)
    {
    case AG_LIMIT_ALARM_ETN2O_LOW:
    case AG_LIMIT_ALARM_ETN2O_HIGH:
        return SUB_PARAM_ETN2O;
    case AG_LIMIT_ALARM_FIN2O_LOW:
    case AG_LIMIT_ALARM_FIN2O_HIGH:
        return SUB_PARAM_FIN2O;
    case AG_LIMIT_ALARM_ETAA1_LOW:
    case AG_LIMIT_ALARM_ETAA1_HIGH:
        return SUB_PARAM_ETAA1;
    case AG_LIMIT_ALARM_FIAA1_LOW:
    case AG_LIMIT_ALARM_FIAA1_HIGH:
        return SUB_PARAM_FIAA1;
    case AG_LIMIT_ALARM_ETAA2_LOW:
    case AG_LIMIT_ALARM_ETAA2_HIGH:
        return SUB_PARAM_ETAA2;
    case AG_LIMIT_ALARM_FIAA2_LOW:
    case AG_LIMIT_ALARM_FIAA2_HIGH:
        return SUB_PARAM_FIAA2;
    case AG_LIMIT_ALARM_ETO2_LOW:
    case AG_LIMIT_ALARM_ETO2_HIGH:
        return SUB_PARAM_ETO2;
    case AG_LIMIT_ALARM_FIO2_LOW:
    case AG_LIMIT_ALARM_FIO2_HIGH:
        return SUB_PARAM_FIO2;
    default:
        return SUB_PARAM_NONE;
    }
}

/**************************************************************************************************
 * alarm id corresponding to alarm priority
 *************************************************************************************************/
AlarmPriority AGLimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * get alarm measure data of physiological parameter
 *************************************************************************************************/
int AGLimitAlarm::getValue(int id)
{
    switch (id)
    {
    case AG_LIMIT_ALARM_ETN2O_LOW:
    case AG_LIMIT_ALARM_ETN2O_HIGH:
        return agParam.getEtData(AG_TYPE_N2O);
    case AG_LIMIT_ALARM_FIN2O_LOW:
    case AG_LIMIT_ALARM_FIN2O_HIGH:
        return agParam.getFiData(AG_TYPE_N2O);
    case AG_LIMIT_ALARM_ETAA1_LOW:
    case AG_LIMIT_ALARM_ETAA1_HIGH:
        return agParam.getEtData(AG_TYPE_AA1);
    case AG_LIMIT_ALARM_FIAA1_LOW:
    case AG_LIMIT_ALARM_FIAA1_HIGH:
        return agParam.getFiData(AG_TYPE_AA1);
    case AG_LIMIT_ALARM_ETAA2_LOW:
    case AG_LIMIT_ALARM_ETAA2_HIGH:
        return agParam.getEtData(AG_TYPE_AA2);
    case AG_LIMIT_ALARM_FIAA2_LOW:
    case AG_LIMIT_ALARM_FIAA2_HIGH:
        return agParam.getFiData(AG_TYPE_AA2);
    case AG_LIMIT_ALARM_ETO2_LOW:
    case AG_LIMIT_ALARM_ETO2_HIGH:
        return agParam.getEtData(AG_TYPE_O2);
    case AG_LIMIT_ALARM_FIO2_LOW:
    case AG_LIMIT_ALARM_FIO2_HIGH:
        return agParam.getFiData(AG_TYPE_O2);
    default:
        return -1;
    }
}

/**************************************************************************************************
 * physiological parameter alarm enable.
 *************************************************************************************************/
bool AGLimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id));
}

/**************************************************************************************************
 * get upper limit of physiological parameter alarm.
 *************************************************************************************************/
int AGLimitAlarm::getUpper(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = UNIT_PERCENT;
    int value = alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
    return value;
}

/**************************************************************************************************
 * get lower limit of physiological parameter alarm.
 *************************************************************************************************/
int AGLimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = UNIT_PERCENT;
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

/**************************************************************************************************
 * compare physiological parameter alarm.
 *************************************************************************************************/
int AGLimitAlarm::getCompare(int value, int id)
{
    if (0 == id % 2)
    {
        if (value < getLower(id))
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (value > getUpper(id))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

/**************************************************************************************************
 * physiological parameter alarm convert string.
 *************************************************************************************************/
const char *AGLimitAlarm::toString(int id)
{
    return AGSymbol::convert((AGLimitAlarmType)id);
}

/**************************************************************************************************
 * alarm notify.
 *************************************************************************************************/
void AGLimitAlarm::notifyAlarm(int /*id*/, bool /*flag*/)
{

}

/**************************************************************************************************
 * destructor
 *************************************************************************************************/
AGLimitAlarm::~AGLimitAlarm()
{

}

/**************************************************************************************************
 * constructor
 *************************************************************************************************/
AGLimitAlarm::AGLimitAlarm()
{

}

/**************************************************************************************************
 *************************************************************************************************/

AGOneShotAlarm *AGOneShotAlarm::_selfObj = NULL;

/**************************************************************************************************
 * one shot alarm source name.
 *************************************************************************************************/
QString AGOneShotAlarm::getAlarmSourceName()
{
    QString str(paramInfo.getParamName(PARAM_AG));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * one shot alarm source number.
 *************************************************************************************************/
int AGOneShotAlarm::getAlarmSourceNR()
{
    return AG_ONESHOT_NR;
}

/**************************************************************************************************
 * one shot alarm id corresponding to waveform ID
 *************************************************************************************************/
WaveformID AGOneShotAlarm::getWaveformID(int /*id*/)
{
    return WAVE_NONE;
}

/**************************************************************************************************
 * one shot alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID AGOneShotAlarm::getSubParamID(int /*id*/)
{
    return SUB_PARAM_NONE;
}

/**************************************************************************************************
 * one shot alarm priority.
 *************************************************************************************************/
AlarmPriority AGOneShotAlarm::getAlarmPriority(int /*id*/)
{
    return ALARM_PRIO_LOW;
}

/**************************************************************************************************
 * one shot alarm type.
 *************************************************************************************************/
AlarmType AGOneShotAlarm::getAlarmType(int /*id*/)
{
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * alarm enable.
 *************************************************************************************************/
bool AGOneShotAlarm::isAlarmed(int id)
{
    return AlarmOneShotIFace::isAlarmed(id);
}

/**************************************************************************************************
 * notify alarm.
 *************************************************************************************************/
void AGOneShotAlarm::notifyAlarm(int /*id*/, bool /*isAlarm*/)
{

}

/**************************************************************************************************
 * one shot alarm convert string.
 *************************************************************************************************/
const char *AGOneShotAlarm::toString(int id)
{
    return AGSymbol::convert((AGOneShotType)id);
}

/**************************************************************************************************
 * enable one shot alarm.
 *************************************************************************************************/
bool AGOneShotAlarm::isAlarmEnable(int /*id*/)
{
    return true;
}

/**************************************************************************************************
 *  remove alarm display information later latch.
 *************************************************************************************************/
bool AGOneShotAlarm::isRemoveAfterLatch(int /*id*/)
{
    return false;
}

/**************************************************************************************************
 * constructor
 *************************************************************************************************/
AGOneShotAlarm::~AGOneShotAlarm()
{

}

/**************************************************************************************************
 * destructor
 *************************************************************************************************/
AGOneShotAlarm::AGOneShotAlarm()
{

}