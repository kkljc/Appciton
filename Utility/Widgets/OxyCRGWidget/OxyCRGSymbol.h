/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/



#pragma once
#include "OxyCRGDefine.h"

// 将定义的枚举转换成符号。
class OxyCRGSymbol
{
public:
    static const char *convert(OxyCRGInterval index)
    {
        if (index > OxyCRG_Interval_8)
        {
            index = OxyCRG_Interval_8;
        }
        static const char *symbol[OxyCRG_Interval_NR] =
        {
            "_1min", "_2min", "_4min", "_8min"
        };
        return symbol[index];
    }

    static const char *convert(OxyCRGTrend index)
    {
        static const char *symbol[OxyCRG_Trend_NR] =
        {
            "RESP", "CO2"
        };
        return symbol[index];
    }

    static const char *convert(TrendTypeOne index)
    {
        static const char *symbol[TRENDONE_NR] =
        {
            "HR_PR", "HR+RR"
        };
        return symbol[index];
    }

    static const char *convert(TrendTypeTwo index)
    {
        static const char *symbol[TRENDTWO_NR] =
        {
            "SPO2"
        };
        return symbol[index];
    }

    static const char *convert(HRLowTypes index)
    {
        static const char *symbol[HR_LOW_NR] =
        {
            "0", "40", "80", "120"
        };
        return symbol[index];
    }

    static const char *convert(HRHighTypes index)
    {
        static const char *symbol[HR_HIGH_NR] =
        {
            "160", "200", "240", "280", "320", "360"
        };
        return symbol[index];
    }


    static const char *convert(RRHighTypes index)
    {
        static const char *symbol[RR_HIGH_NR] =
        {
            "40", "80", "120", "160"
        };
        return symbol[index];
    }

    static const char *convert(RRLowTypes index)
    {
        static const char *symbol[RR_LOW_NR] =
        {
            "0", "40"
        };
        return symbol[index];
    }

    static const char *convert(SPO2HighTypes index)
    {
        static const char *symbol[SPO2_HIGH_NR] =
        {
            "100"
        };
        return symbol[index];
    }

    static const char *convert(SPO2LowTypes index)
    {
        static const char *symbol[SPO2_LOW_NR] =
        {
            "60", "68", "76", "84", "92"
        };
        return symbol[index];
    }

    static const char *convert(CO2HighTypes index)
    {
        static const char *symbol[CO2_HIGH_NR] =
        {
            "15", "20", "25", "40", "50", "60", "80"
        };
        return symbol[index];
    }

    static const char *convert(CO2LowTypes index)
    {
        static const char *symbol[CO2_LOW_NR] =
        {
            "0"
        };
        return symbol[index];
    }
};
