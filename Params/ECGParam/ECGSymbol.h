#pragma once
#include <QString>
#include "ECGDefine.h"

// 将定义的枚举转换成符号。
class ECGSymbol
{
public:
    static const char *convert(ECGLeadMode index)
    {
        static const char *symbol[ECG_LEAD_MODE_NR] =
        {
            "ECG3-Lead", "ECG5-Lead", "ECG12-Lead"
        };

        return symbol[index];
    }

    static const char *convert(ECGDisplayMode index)
    {
        static const char *symbol[ECG_DISPLAY_NR] =
        {
            "ECGNormal", "ECG12-Lead-Full"
        };

        return symbol[index];
    }

    static const char *convert(ECGLead index, ECGLeadNameConvention convention,
                               bool is12L = false, bool isCabrera = false)
    {
        static const char *symbol[ECG_CONVENTION_NR][ECG_LEAD_NR] =
        {
            {
                "I",  "II", "III", "aVR", "aVL", "aVF",
                "V1", "V2",  "V3", "V4",  "V5",  "V6"
            },

            {
                "I",  "II", "III", "aVR", "aVL", "aVF",
                "C1", "C2",  "C3", "C4",  "C5",  "C6",
            }
        };

        static const char *ecg12LAVR = "-aVR";

        if ((index == ECG_LEAD_AVR) && is12L && isCabrera)
        {
            return ecg12LAVR;
        }
        else
        {
            return symbol[convention][index];
        }
    }

    static const char *convert(ECGGain index)
    {
        static const char *symbol[ECG_GAIN_NR] =
        {
            "0.125 cm/mV", "0.25 cm/mV", "0.5 cm/mV", "1.0 cm/mV", "1.5 cm/mV", "2.0 cm/mV", "3.0 cm/mV", "AUTO"
//            "0.5 cm/mV", "1.0 cm/mV", "1.5 cm/mV", "2.0 cm/mV", "3.0 cm/mV", "AUTO"
        };

        return symbol[index];
    }

    static const char *convert(ECGST index)
    {
        static const char *symbol[ECG_ST_NR] =
        {
            "I",  "II", "III", "aVR", "aVL", "aVF",
            "V1", "V2", "V3", "V4", "V5", "V6"
        };

        return symbol[index];
    }

    static const char *convert(ECGPaceMode index)
    {
        static const char *symbol[ECG_PACE_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    static const char *convert(ECGAlarmSource src)
    {
        static const char *symbol[ECG_ALARM_SRC_NR] =
        {
            "HR",
            "PR",
            "AUTO",
        };
        return symbol[src];
    }

    static const char *convert(ECGSweepSpeed index)
    {
        static const char *symbol[ECG_SWEEP_SPEED_NR] =
        {
            "12.5 mm/s", "25.0 mm/s", "50.0 mm/s"
        };

        return symbol[index];
    }

    static const char *convert(ECGBandwidth index)
    {
        // add one more item to store multiple BW string
        static const char *symbol[ECG_BANDWIDTH_NR + 1] =
        {
            "0.67-20 Hz",
            "0.67-40 Hz",
            "0.525-40 Hz",
            "0.525-150 Hz",
            "Multiple BW",
        };

        return symbol[index];
    }

    static const char *convert(ECGFilterMode mode)
    {
        static const char *symbol[ECG_FILTERMODE_NR] =
        {
            "Monitor",
            "Diagnostic",
            "Surgery",
            "ST",
        };
        return symbol[mode];
    }

    static const char *convert(ECGNotchFilter index)
    {
        static const char *symbol[ECG_NOTCH_NR] =
        {
            "Off", "50Hz", "60Hz", "50&60Hz"
        };

        return symbol[index];
    }

    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(ECGDupLimitAlarmType index)
    {
        static const char *symbol[ECG_DUP_LIMIT_ALARM_NR] =
        {
            "HRLow", "HRHigh", "SPO2PRLow", "SPO2PRHigh"
        };

        return symbol[index];
    }

    static const char *convert(ECGOneShotType index, ECGLeadNameConvention convention)
    {
        static const char *symbol[ECG_ONESHOT_NR] =
        {
            "ECGArrASYSTOLE",
            "ECGArrVFIBVTAC",
            "CheckPatient",

            "ECGLeadOff",         "ECGV1LeadOff",
            "ECGV2LeadOff",       "ECGV3LeadOff",
            "ECGV4LeadOff",       "ECGV5LeadOff",
            "ECGV6LeadOff",       "ECG12LTransfersuccess",
            "ECG12LTransferfail", "ECGOverLoad",
            "ECGCommunicationStop"
        };

        static const char *leadOff[6] =
        {
            "ECGC1LeadOff",
            "ECGC2LeadOff",
            "ECGC3LeadOff",
            "ECGC4LeadOff",
            "ECGC5LeadOff",
            "ECGC6LeadOff"
        };

        if (index >= ECG_ONESHOT_ALARM_V1_LEADOFF && index <= ECG_ONESHOT_ALARM_V6_LEADOFF)
        {
            if (convention == ECG_CONVENTION_AAMI)
            {
                return symbol[index];
            }
            else
            {
                return leadOff[index - ECG_ONESHOT_ALARM_V1_LEADOFF];
            }
        }
        else
        {
            return symbol[index];
        }
    }

    static const char *convert(ECGLeadNameConvention index)
    {
        static const char *symbol[ECG_CONVENTION_NR] =
        {
            "AAMI",
            "IEC"
        };

        return symbol[index];
    }

    static const QString &convert(Display12LeadFormat index)
    {
        static const QString symbol[DISPLAY_12LEAD_NR] =
        {
            "Standard", "Cabrera"
        };

        return symbol[index];
    }
};