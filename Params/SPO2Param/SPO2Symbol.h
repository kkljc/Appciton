#pragma once
#include "SPO2Define.h"

// 将定义的枚举转换成符号。
class SPO2Symbol
{
public:
    static const char *convert(SPO2Sensitive index)
    {
        static const char *symbol[SPO2_SENS_NR] =
        {
            "SPO2SensLow", "SPO2SensMed", "SPO2SensHigh"
        };
        return symbol[index];
    }

    static const char *convert(SPO2SweepSpeed index)
    {
        static const char *symbol[SPO2_SWEEP_SPEED_NR] =
        {
            "12.5 mm/s", "25.0 mm/s", "50.0 mm/s"
        };
        return symbol[index];
    }

    static const char *convert(SPO2PRDisplay index)
    {
        static const char *symbol[SPO2_PR_DISPLAY_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(SPO2LimitAlarmType index)
    {
        static const char *symbol[SPO2_LIMIT_ALARM_NR] =
        {
            "SPO2Low", "SPO2High"
        };
        return symbol[index];
    }

    static const char *convert(SPO2SMARTPLUSETONE index)
    {
        static const char *symbol[SPO2_SMART_PLUSE_TONE_NR] =
        {
            "On",
            "Off",
        };
        return symbol[index];
    }

    static const char *convert(SPO2OneShotType index)
    {
        static const char *symbol[SPO2_ONESHOT_NR] =
        {
            "SPO2CheckSensor",
            "SPO2CableOff",
            "SPO2FingerOff",
            "SPO2LEDFault",
            "SPO2CommunicationStop",
            "SPO2SignalWeak",
            "SPO2SignalSaturation",
            "SPO2LowPerfusion"
        };
        return symbol[index];
    }

    static const char *convert(SPO2Gain index)
    {
        static const char *symbol[SPO2_GAIN_NR] =
        {
            "x1.0", "x2.0", "x4.0", "x8.0"
        };

        return symbol[index];
    }
};