#pragma once

// 交直流模式
enum PowerStat
{
    POWER_UNKNOWN,      // 未知
    POWER_AC,           // 交流
    POWER_BAT,          // 电池
    POWER_AC_BAT,       // 交直流
    POWER_BAT_ERROR,    // 电池故障
    POWER_AC_BAT_ERROR, // 交直流故障
    POWER_NR
};

// 界面类型
enum UserFaceType
{
    UFACE_MONITOR_STANDARD,
    UFACE_MONITOR_12LEAD,
    UFACE_MONITOR_OXYCRG,
    UFACE_MONITOR_TREND,
    UFACE_MONITOR_BIGFONT,
    UFACE_MONITOR_CUSTOM,
    UFACE_MONITOR_UNKNOW,
    UFACE_NR
};

// 背光亮度等级。
enum BrightnessLevel
{
    BRT_LEVEL_0,
    BRT_LEVEL_1,
    BRT_LEVEL_2,
    BRT_LEVEL_3,
    BRT_LEVEL_4,
    BRT_LEVEL_5,
    BRT_LEVEL_6,
    BRT_LEVEL_7,
    BRT_LEVEL_8,
    BRT_LEVEL_9,
    BRT_LEVEL_NR
};

// 错误警告码
enum ErrorWaringCode
{
    ERR_CODE_NONE,              // 正常，无错误
    ERR_CODE_BAT_COMM_FAULT,    // 电池通讯错误
    ERR_CODE_BAT_ERROR,         // 电池错误
    ERR_CODE_NR
};

class SystemSymbol
{
public:
    static const char *convert(UserFaceType index)
    {
        static const char *symbol[UFACE_NR] =
        {
            "UserFaceStandard", "UserFace12Lead","UserFaceOxyCRG",
            "UserFaceTrend", "UserFaceBigFont", "UserFaceCustom", "UserFaceUnknow"
        };
        return symbol[index];
    }
};