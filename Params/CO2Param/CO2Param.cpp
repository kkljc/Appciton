/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#include "CO2Param.h"
#include "CO2Alarm.h"
#include "PatientManager.h"
#include "CO2ProviderIFace.h"
#include "CO2TrendWidget.h"
#include "CO2WaveWidget.h"
#include "ConfigManager.h"
#include "WaveformCache.h"
#include "RESPParam.h"
#include "ParamManager.h"
#include "SystemManager.h"
#include "RESPDupParam.h"
#include "SoftKeyManager.h"
#include "QApplication"
#include "LayoutManager.h"
#include "OxyCRGCO2WaveWidget.h"
#include "AlarmSourceManager.h"
#include "O2ParamInterface.h"
#include "IConfig.h"
#include "Components/DataUploader/BLMMessageDefine.h"

CO2Param *CO2Param::_selfObj = NULL;


class CO2ParamPrivate
{
public:
   explicit CO2ParamPrivate(CO2Param *p)
            : q_ptr(p),
              provider(NULL),
              trendWidget(NULL),
              waveWidget(NULL),
              etco2Value(InvData()),
              fico2Value(InvData()),
              awRRValue(InvData()),
              etco2MaxVal(0),
              etco2MinVal(0),
              brVaule(InvData()),
              baro(1013),
              connectedProvider(false),
              co2Switch(false),
              oxyCRGCO2Wave(NULL),
              calibrateChannel(0),
              calibrateResult(false),
              calibrateReply(false),
              disableZero(0),
              o2CompensationEnable(false),
              n2oCompensationEnable(false),
              startCal(false)
    {
    }

    /**
     * @brief setWaveformSpeed
     * @param speed
     */
    void setWaveformSpeed(CO2SweepSpeed speed);

    /**
     * @brief setWaveformZoom
     * @param zoom
     */
    void setWaveformZoom(CO2DisplayZoom zoom);

    CO2Param *q_ptr;
    CO2ProviderIFace *provider;
    CO2TrendWidget *trendWidget;
    CO2WaveWidget *waveWidget;

    short  etco2Value;
    short  fico2Value;
    short  awRRValue;
    short  etco2MaxVal;
    short  etco2MinVal;
    short  brVaule;
    short  baro;
    bool   connectedProvider;
    bool   co2Switch;

    OxyCRGCO2WaveWidget *oxyCRGCO2Wave;

    int calibrateChannel;
    bool calibrateResult;
    bool calibrateReply;

    int disableZero;
    bool o2CompensationEnable;
    bool n2oCompensationEnable;

    bool startCal;
};
/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
void CO2ParamPrivate::setWaveformSpeed(CO2SweepSpeed speed)
{
    if (waveWidget == NULL)
    {
        return;
    }
    switch (speed)
    {
    case CO2_SWEEP_SPEED_62_5:
        waveWidget->setWaveSpeed(6.25);
        break;
    case CO2_SWEEP_SPEED_125:
        waveWidget->setWaveSpeed(12.5);
        break;
    case CO2_SWEEP_SPEED_250:
        waveWidget->setWaveSpeed(25);
        break;
    case CO2_SWEEP_SPEED_500:
        waveWidget->setWaveSpeed(50);
        break;
    default:
        break;
    }

    QStringList currentWaveforms = layoutManager.getDisplayedWaveforms();
    int i = 0;
    int size = currentWaveforms.size();
    for (; i < size; i++)
    {
        if (currentWaveforms[i] == "CO2WaveWidget")
        {
            layoutManager.resetWave();
            break;
        }
    }
}

/**************************************************************************************************
 * ??????????????????
 *************************************************************************************************/
void CO2ParamPrivate::setWaveformZoom(CO2DisplayZoom zoom)
{
    if (provider == NULL)
    {
        return;
    }
    if (waveWidget == NULL)
    {
        return;
    }
    switch (zoom)
    {
    case CO2_DISPLAY_ZOOM_4:
        waveWidget->setValueRange(0, (provider->getCO2MaxWaveform() * 4 + 19) / 20);
        waveWidget->setRuler(zoom);
        break;

    case CO2_DISPLAY_ZOOM_8:  // 0???8/15????????????14?????????????????????????????????????????????
        waveWidget->setValueRange(0, (provider->getCO2MaxWaveform() * 8 + 19) / 20);
        waveWidget->setRuler(zoom);
        break;

    case CO2_DISPLAY_ZOOM_12:
        waveWidget->setValueRange(0, (provider->getCO2MaxWaveform() * 12 + 19) / 20);
        waveWidget->setRuler(zoom);
        break;

    case CO2_DISPLAY_ZOOM_20:
        waveWidget->setValueRange(0, provider->getCO2MaxWaveform());
        waveWidget->setRuler(zoom);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * ??????????????????
 *************************************************************************************************/
void CO2Param::initParam(void)
{
    int onoff = 1;
    currentConfig.getNumValue("CO2|CO2ModeDefault", onoff);
    d_ptr->co2Switch = (1 == onoff) ? true : false;
}

/**************************************************************************************************
 * ??????DEMO?????????
 *************************************************************************************************/
void CO2Param::handDemoWaveform(WaveformID id, short data)
{
    int flag = 0;
    if (id != WAVE_CO2)
    {
        return;
    }

    if (preProcessor)
    {
        int dataInt = data;
        preProcessor->preProcessWave(WAVE_CO2, &dataInt, &flag);
        data = dataInt;
    }

    if (!getCO2Switch())
    {
        data = 0;
        flag = 0x4000;
    }
    if (NULL != d_ptr->waveWidget)
    {
        d_ptr->waveWidget->addData(data, flag);
    }

    if (d_ptr->oxyCRGCO2Wave)
    {
        d_ptr->oxyCRGCO2Wave->addWaveData(data);
    }
    waveformCache.addData((WaveformID)id, data);
}

/**************************************************************************************************
 * ??????DEMO?????????
 *************************************************************************************************/
void CO2Param::handDemoTrendData(void)
{
//    d_ptr->etco2Value = qrand() % 80 + 1;
//    d_ptr->fico2Value = qrand() % 80;
    if (getCO2Switch())
    {
        d_ptr->etco2Value = 50;
        d_ptr->fico2Value = 0;
        d_ptr->awRRValue = 20;
        d_ptr->brVaule = 20;
        d_ptr->baro = 1013;
    }
    else
    {
        d_ptr->etco2Value = InvData();
        d_ptr->fico2Value = InvData();
        d_ptr->awRRValue = InvData();
        d_ptr->brVaule = InvData();
    }

    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setEtCO2Value(d_ptr->etco2Value);
        d_ptr->trendWidget->setFiCO2Value(d_ptr->fico2Value);
        d_ptr->trendWidget->setawRRValue(d_ptr->awRRValue);
    }
    setBR(d_ptr->brVaule);
}

void CO2Param::exitDemo()
{
    d_ptr->etco2Value = InvData();
    d_ptr->fico2Value = InvData();
    d_ptr->awRRValue =  InvData();
    d_ptr->brVaule = InvData();

    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setEtCO2Value(InvData());
        d_ptr->trendWidget->setFiCO2Value(InvData());
        d_ptr->trendWidget->setawRRValue(InvData());
    }
    setBR(InvData());
}

/**************************************************************************************************
 * ?????????????????????????????????
 *************************************************************************************************/
void CO2Param::getAvailableWaveforms(QStringList *waveforms, QStringList *waveformShowName, int)
{
    waveforms->clear();
    waveformShowName->clear();

    if (NULL != d_ptr->waveWidget)
    {
        waveforms->append(d_ptr->waveWidget->name());
        waveformShowName->append(d_ptr->waveWidget->getTitle());
    }
}

/**************************************************************************************************
 * ?????????????????????????????????
 *************************************************************************************************/
QString CO2Param::getTrendWindowName()
{
    if (NULL != d_ptr->trendWidget)
    {
        return d_ptr->trendWidget->name();
    }
    return QString();
}

/**************************************************************************************************
 * ?????????????????????????????????
 *************************************************************************************************/
QString CO2Param::getWaveWindowName()
{
    if (NULL != d_ptr->waveWidget)
    {
        return d_ptr->waveWidget->name();
    }
    return QString();
}

/**************************************************************************************************
 * ????????? ?????????????????????
 *************************************************************************************************/
short CO2Param::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ETCO2:
        return getEtCO2();

    case SUB_PARAM_FICO2:
        return getFiCO2();

    case SUB_PARAM_AWRR:
        return getAWRR();
    default:
        return InvData();
    }
}

/**************************************************************************************************
 * ????????? ?????????????????????
 *************************************************************************************************/
void CO2Param::showSubParamValue()
{
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->showValue();
    }
}

/**************************************************************************************************
 * ????????????????????????
 *************************************************************************************************/
UnitType CO2Param::getCurrentUnit(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_AWRR:
        return UNIT_BPM;
        break;
    default:
        break;
    }

    return getUnit();
}

QVariantMap CO2Param::getTrendVariant(int id)
{
    QVariantMap map;
    if (id == BLM_TREND_PARAM_ETCO2)
    {
        map["ParamID"] = BLM_PARAM_CO2;
        map["TrendID"] = BLM_TREND_PARAM_ETCO2;
        map["Value"] = d_ptr->etco2Value;
        map["Status"] = d_ptr->etco2Value == InvData() ? 1 : 0;
    }
    else if (id == BLM_TREND_PARAM_FICO2)
    {
        map["ParamID"] = BLM_PARAM_CO2;
        map["TrendID"] = BLM_TREND_PARAM_FICO2;
        map["Value"] = d_ptr->fico2Value;
        map["Status"] = d_ptr->fico2Value == InvData() ? 1 : 0;
    }
    return map;
}

/**************************************************************************************************
 * ???????????????????????????
 *************************************************************************************************/
void CO2Param::setProvider(CO2ProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    if (d_ptr->waveWidget == NULL)
    {
        return;
    }

    d_ptr->provider = provider;

    QString tile = d_ptr->waveWidget->getTitle();

    // ????????????????????????
    waveformCache.registerSource(WAVE_CO2, d_ptr->provider->getCO2WaveformSample(),
                                 0, d_ptr->provider->getCO2MaxWaveform(), tile, d_ptr->provider->getCO2BaseLine());

    // ?????????????????????
    d_ptr->waveWidget->setDataRate(d_ptr->provider->getCO2WaveformSample());
    d_ptr->setWaveformZoom(getDisplayZoom());
    d_ptr->oxyCRGCO2Wave->setDataRate(d_ptr->provider->getCO2WaveformSample());
    // ???????????????
    d_ptr->provider->setApneaTimeout(getApneaTime());

    // ??????N2O?????????
    d_ptr->provider->setN2OCompensation(getCompensation(CO2_COMPEN_N2O));

    // ??????O2?????????
    d_ptr->provider->setO2Compensation(getCompensation(CO2_COMPEN_O2));
}

/**************************************************************************************************
 * ???????????????
 *************************************************************************************************/
void CO2Param::reset()
{
    if (NULL == d_ptr->provider)
    {
        return;
    }

    // ???????????????
    d_ptr->provider->setApneaTimeout(getApneaTime());

    // ??????N2O?????????
    d_ptr->provider->setN2OCompensation(getCompensation(CO2_COMPEN_N2O));

    // ??????O2?????????
    d_ptr->provider->setO2Compensation(getCompensation(CO2_COMPEN_O2));
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
void CO2Param::setTrendWidget(CO2TrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    d_ptr->trendWidget = trendWidget;
    d_ptr->trendWidget->setFiCO2Display(getFICO2Display());
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
void CO2Param::setWaveWidget(CO2WaveWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }

    d_ptr->waveWidget = waveWidget;
    d_ptr->setWaveformSpeed(getSweepSpeed());
    d_ptr->waveWidget->setWaveformMode(getSweepMode());
}

/**************************************************************************************************
 * ??????EtCO2???
 *************************************************************************************************/
short CO2Param::getEtCO2(void)
{
    return d_ptr->etco2Value;
}

/**************************************************************************************************
 * ??????EtCO2???
 *************************************************************************************************/
void CO2Param::setEtCO2(short etco2)
{
    if (!(isEnabled() && d_ptr->co2Switch))
    {
        etco2 = InvData();
    }

#ifdef ENABLE_O2_APNEASTIMULATION
    int mul = paramInfo.getMultiOfSubParam(SUB_PARAM_ETCO2);
    int etco2mmhg = (etco2 * 1.0) / mul * d_ptr->baro / 1000 * 7.5 + 0.5;
    setRespApneaStimulation((etco2 != InvData() && (etco2mmhg < 7)));
#endif

    d_ptr->etco2Value = etco2;
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setEtCO2Value(etco2);
        paramUpdateTimer->start(PARAM_UPDATE_TIMEOUT);
    }
}

/**************************************************************************************************
 * ??????FiCO2???
 *************************************************************************************************/
short CO2Param::getFiCO2(void)
{
    return d_ptr->fico2Value;
}

/**************************************************************************************************
 * ??????FiCO2???
 *************************************************************************************************/
void CO2Param::setFiCO2(short fico2)
{
    if (!(isEnabled() && d_ptr->co2Switch))
    {
        fico2 = InvData();
    }

    d_ptr->fico2Value = fico2;
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setFiCO2Value(fico2);
    }
}

/**************************************************************************************************
 * ??????BR???
 *************************************************************************************************/
void CO2Param::setBR(short br)
{
    if (!(isEnabled() && d_ptr->co2Switch))
    {
        br = InvData();
    }

    // ?????????????????????
    if (br > respParam.getRRMeasureMaxRange()
            || br < respParam.getRRMeasureMinRange())
    {
        br = InvData();
    }

    // DAVID????????????????????????148???149???????????????????????????????????1???????????????????????????????????????????????????????????????
//    if (br >= 148 && br <=149)
//    {
//        br += 1;
//    }

    respDupParam.updateBR(br);
}

void CO2Param::setRR(short rr)
{
    if (!(isEnabled() && d_ptr->co2Switch))
    {
        rr = InvData();
    }

    // ?????????????????????
    if (rr > respParam.getRRMeasureMaxRange()
            || rr < respParam.getRRMeasureMinRange())
    {
        rr = InvData();
    }

    // DAVID????????????????????????148???149???????????????????????????????????1???????????????????????????????????????????????????????????????
//    if (rr >= 148 && rr <=149)
//    {
//        rr += 1;
//    }

    d_ptr->awRRValue = rr;
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setawRRValue(rr);
    }
}

short CO2Param::getAWRR()
{
    return d_ptr->awRRValue;
}

/**************************************************************************************************
 * ??????Baro???
 *************************************************************************************************/
short CO2Param::getBaro(void)
{
    return d_ptr->baro;
}

/**************************************************************************************************
 * ??????Baro???
 *************************************************************************************************/
void CO2Param::setBaro(short baro)
{
    d_ptr->baro = baro;
}

/**************************************************************************************************
 * ??????????????????????????????????????????????????????
 *************************************************************************************************/
void CO2Param::verifyApneanTime(ApneaAlarmTime time)
{
    if (getApneaTime() != time)
    {
        if (d_ptr->provider != NULL)
        {
            d_ptr->provider->setApneaTimeout(getApneaTime());
        }
    }
}

/**************************************************************************************************
 * ??????????????????????????????????????????????????????
 *************************************************************************************************/
void CO2Param::verifyWorkMode(CO2WorkMode mode)
{
    if (mode == CO2_WORK_SELFTEST)
    {
        return;
    }

    if (NULL == d_ptr->provider)
    {
        return;
    }

    // ????????????????????????????????????sleep???
    if (getCO2Switch() && (mode == C02_WORK_SLEEP))
    {
        setModuleWorkMode(CO2_WORK_MEASUREMENT);
    }

    // ?????????????????????????????????????????????
    if (!getCO2Switch() && (mode == CO2_WORK_MEASUREMENT))
    {
        setModuleWorkMode(C02_WORK_SLEEP);
    }
}

/**************************************************************************************************
 * ????????????????????????????????????
 *************************************************************************************************/
void CO2Param::setConnected(bool isConnected)
{
    if (d_ptr->connectedProvider == isConnected)
    {
        return;
    }

    d_ptr->connectedProvider = isConnected;

    QString co2Trend = getTrendWindowName();
    QString co2Wave = getWaveWindowName();

    int needUpdate = 0;
    if (isConnected)
    {
        this->enable();

        // update to show CO2 info
        needUpdate |= layoutManager.setWidgetLayoutable(co2Trend, true);
        needUpdate |= layoutManager.setWidgetLayoutable(co2Wave, true);
        if (needUpdate)
        {
            layoutManager.updateLayout();
        }
        // ??????co2??????????????????
        softkeyManager.setKeyTypeAvailable(SOFT_BASE_KEY_CO2_CALIBRATION, true);
        softkeyManager.setKeyTypeAvailable(SOFT_BASE_KEY_CO2_HANDLE, true);
        softkeyManager.refreshPage();
    }
    else
    {
        disable();
        d_ptr->baro = 1013;
        setEtCO2(InvData());
        setFiCO2(InvData());
        setBR(InvData());
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_CO2);
        if (alarmSource)
        {
            alarmSource->clear();
        }
        // update to show CO2 info
        needUpdate |= layoutManager.setWidgetLayoutable(co2Trend, false);
        needUpdate |= layoutManager.setWidgetLayoutable(co2Wave, false);
        if (needUpdate)
        {
            layoutManager.updateLayout();
        }
        // ??????co2??????????????????
        softkeyManager.setKeyTypeAvailable(SOFT_BASE_KEY_CO2_CALIBRATION, false);
        softkeyManager.setKeyTypeAvailable(SOFT_BASE_KEY_CO2_HANDLE, false);
        softkeyManager.refreshPage();
    }

    emit connectStatusUpdated(isConnected);
}

/**************************************************************************************************
 * ????????????????????????Provider?????????Provider??????????????????
 *************************************************************************************************/
bool CO2Param::isConnected(void)
{
    return d_ptr->connectedProvider;
}

/**************************************************************************************************
 * ??????????????????
 *************************************************************************************************/
void CO2Param::addWaveformData(short wave, bool invalid)
{
    int flag = 0;
    if (invalid || !d_ptr->co2Switch)
    {
        flag = 0x4000;
        wave = 0;
    }

    if (preProcessor)
    {
        int waveInt = wave;
        preProcessor->preProcessWave(WAVE_CO2, &waveInt, &flag);
        wave = waveInt;
    }

    if (d_ptr->waveWidget != NULL)
    {
        d_ptr->waveWidget->addData(wave, flag);
    }

    if (d_ptr->oxyCRGCO2Wave)
    {
        d_ptr->oxyCRGCO2Wave->addWaveData(wave);
    }

    waveformCache.addData(WAVE_CO2, (flag << 16) | (wave & 0xFFFF));
}

/**************************************************************************************************
 * ??????OneShot?????????
 *************************************************************************************************/
void CO2Param::setOneShotAlarm(CO2OneShotType t, bool status)
{
    // ?????????CO2?????????ON??????????????????
    if (d_ptr->co2Switch || t == CO2_ONESHOT_ALARM_STANDBY)
    {
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_CO2);
        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(t, status);
        }
    }
}

/**************************************************************************************************
 * ????????????????????????CO2???????????????
 *************************************************************************************************/
void CO2Param::setOxyCRGCO2Widget(OxyCRGCO2WaveWidget *p)
{
    if (p)
    {
        d_ptr->oxyCRGCO2Wave = p;
    }
}


/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
void CO2Param::noticeLimitAlarm(int id, bool flag)
{
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->isAlarm(id, flag);
    }
}

void CO2Param::updateUnit()
{
    if (d_ptr->trendWidget != NULL)
    {
        d_ptr->trendWidget->setUNit(getUnit());
        d_ptr->trendWidget->updateLimit();
    }

    if (d_ptr->waveWidget != NULL)
    {
        d_ptr->waveWidget->setRuler(getDisplayZoom());
    }
}

bool CO2Param::setModuleWorkMode(CO2WorkMode mode)
{
    if (mode == CO2_WORK_SELFTEST)
    {
        return false;
    }
    if (d_ptr->provider == NULL)
    {
        return false;
    }
    if (mode == C02_WORK_SLEEP)
    {
        setCO2Switch(false);
        d_ptr->provider->setWorkMode(mode);
        softkeyManager.refreshCO2Key(false);
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_STANDBY, true);
        return true;
    }

    if (mode == CO2_WORK_MEASUREMENT)
    {
        setCO2Switch(true);
        d_ptr->provider->setWorkMode(mode);
        softkeyManager.refreshCO2Key(true);
        co2Param.setOneShotAlarm(CO2_ONESHOT_ALARM_STANDBY, false);
        return true;
    }
    return false;
}

void CO2Param::sendCalibrateData(int value)
{
    d_ptr->calibrateChannel = value;
    d_ptr->provider->sendCalibrateData(value);
}

void CO2Param::setCalibrateData(const unsigned char *packet)
{
    if (d_ptr->calibrateChannel == packet[1])
    {
        d_ptr->calibrateReply = true;
        d_ptr->calibrateResult = packet[2];
    }
}

bool CO2Param::getCalibrateResult()
{
    return d_ptr->calibrateResult;
}

bool CO2Param::getCalibrateReply()
{
    bool reply = d_ptr->calibrateReply;
    if (reply)
    {
        d_ptr->calibrateReply = false;
    }
    return reply;
}

void CO2Param::setZeroStatus(CO2DisableZeroReason reason, bool status)
{
    bool reasonSta = d_ptr->disableZero & (1 << reason);
    if (reasonSta != status)
    {
        bool prvSta = d_ptr->disableZero;
        if (status)
        {
            d_ptr->disableZero |= 1 << reason;
        }
        else
        {
            d_ptr->disableZero &= ~(1 << reason);
        }
        bool curSta = d_ptr->disableZero;
        if (prvSta != curSta)
        {
//            softkeyManager.setKeyTypeAvailable(SOFT_BASE_KEY_CO2_CALIBRATION, !curSta);
            emit updateZeroSta(curSta);
            d_ptr->startCal = false;
        }
    }
}

bool CO2Param::getDisableZeroStatus()
{
    return d_ptr->disableZero;
}

void CO2Param::enableCompensation(CO2Compensation gas, bool enable)
{
    if (gas == CO2_COMPEN_O2)
    {
        if (enable != d_ptr->o2CompensationEnable)
        {
            d_ptr->o2CompensationEnable = enable;
            emit updateCompensation(CO2_COMPEN_O2, enable);
        }
    }
    else if (gas == CO2_COMPEN_N2O)
    {
        if (enable != d_ptr->n2oCompensationEnable)
        {
            d_ptr->n2oCompensationEnable = enable;
            emit updateCompensation(CO2_COMPEN_N2O, enable);
        }
    }
}

bool CO2Param::getCompensationEnabled(CO2Compensation gas)
{
    if (gas == CO2_COMPEN_O2)
    {
        return d_ptr->o2CompensationEnable;
    }
    else if (gas == CO2_COMPEN_N2O)
    {
        return d_ptr->n2oCompensationEnable;
    }
    return false;
}

CO2ModuleType CO2Param::getCo2ModuleType() const
{
    if (d_ptr->provider)
    {
        return d_ptr->provider->getCo2ModuleType();
    }
    return MODULE_CO2_NR;
}

/**************************************************************************************************
 * ?????????
 *************************************************************************************************/
void CO2Param::zeroCalibration(void)
{
    if (d_ptr->provider != NULL)
    {
        d_ptr->provider->zeroCalibration();
        d_ptr->startCal = true;
    }
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
void CO2Param::setApneaTime(ApneaAlarmTime t)
{
    if (d_ptr->provider != NULL)
    {
        d_ptr->provider->setApneaTimeout(t);
    }
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
ApneaAlarmTime CO2Param::getApneaTime(void)
{
    int t = APNEA_ALARM_TIME_20_SEC;
    currentConfig.getNumValue("CO2|ApneaTime", t);
    return (ApneaAlarmTime)t;
}

/**************************************************************************************************
 * ??????LTA???????????????
 *************************************************************************************************/
bool CO2Param::getAwRRSwitch(void)
{
    int enable = 0;
    QString path = "AlarmSource|";
    path += paramInfo.getSubParamName(SUB_PARAM_RR_BR);
    currentConfig.getNumAttr(path, "Enable", enable);

    return enable;
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
void CO2Param::setSweepSpeed(CO2SweepSpeed speed)
{
    currentConfig.setNumValue("CO2|SweepSpeed", static_cast<int>(speed));
    d_ptr->setWaveformSpeed(speed);
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
CO2SweepSpeed CO2Param::getSweepSpeed(void)
{
    int speed = CO2_SWEEP_SPEED_62_5;
    currentConfig.getNumValue("CO2|SweepSpeed", speed);
    return (CO2SweepSpeed)speed;
}

void CO2Param::setSweepMode(CO2SweepMode mode)
{
    currentConfig.setNumValue("CO2|CO2SweepMode", static_cast<int>(mode));
    if (d_ptr->waveWidget)
    {
        d_ptr->waveWidget->setWaveformMode(mode);
    }
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
CO2SweepMode CO2Param::getSweepMode(void)
{
    int mode = CO2_SWEEP_MODE_CURVE;
    currentConfig.getNumValue("CO2|CO2SweepMode", mode);
    return (CO2SweepMode)mode;
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
void CO2Param::setCompensation(CO2Compensation which, int v)
{
    if (NULL == d_ptr->provider)
    {
        return;
    }

    QString path("CO2|");
    if (which == CO2_COMPEN_O2)
    {
        path += "O2Compensation";
        d_ptr->provider->setO2Compensation(v);
    }
    else
    {
        path += "N2OCompensation";
        d_ptr->provider->setN2OCompensation(v);
    }

    currentConfig.setNumValue(path, v);
}

/**************************************************************************************************
 * ?????????????????????
 *************************************************************************************************/
int CO2Param::getCompensation(CO2Compensation which)
{
    QString path("CO2|");
    if (which == CO2_COMPEN_O2)
    {
        path += "O2Compensation";
    }
    else
    {
        path += "N2OCompensation";
    }

    int compensation = 0;
    currentConfig.getNumValue(path, compensation);
    return compensation;
}

/**************************************************************************************************
 * ???????????????????????????
 *************************************************************************************************/
void CO2Param::setDisplayZoom(CO2DisplayZoom zoom)
{
    currentConfig.setNumValue("CO2|DisplayZoom", static_cast<int>(zoom));
    d_ptr->setWaveformZoom(zoom);
}

void CO2Param::updateDisplayZoom()
{
    int index = 0;
    currentConfig.getNumValue("CO2|DisplayZoom", index);
    d_ptr->setWaveformZoom(static_cast<CO2DisplayZoom>(index));
}

/**************************************************************************************************
 * ???????????????????????????
 *************************************************************************************************/
CO2DisplayZoom CO2Param::getDisplayZoom(void)
{
    int zoom = CO2_DISPLAY_ZOOM_4;
    currentConfig.getNumValue("CO2|DisplayZoom", zoom);
    return (CO2DisplayZoom)zoom;
}

/**************************************************************************************************
 * ??????FiCO2???????????????
 *************************************************************************************************/
void CO2Param::setFiCO2Display(CO2FICO2Display disp)
{
    currentConfig.setNumValue("CO2|FiCO2Display", static_cast<int>(disp));
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setFiCO2Display(disp);
    }
}

void CO2Param::updateFiCO2Display()
{
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setFiCO2Display(getFICO2Display());
    }
}

/**************************************************************************************************
 * ??????FiCO2???????????????
 *************************************************************************************************/
CO2FICO2Display CO2Param::getFICO2Display(void)
{
    int onoff = CO2_FICO2_DISPLAY_OFF;
    currentConfig.getNumValue("CO2|FiCO2Display", onoff);
    return (CO2FICO2Display)onoff;
}

/**************************************************************************************************
 * ???????????????
 *************************************************************************************************/
UnitType CO2Param::getUnit(void)
{
    int unit = UNIT_PERCENT;
    systemConfig.getNumValue("Unit|CO2Unit", unit);
    return static_cast<UnitType>(unit);
}

void CO2Param::setCO2Switch(bool on)
{
    int index = on;
    currentConfig.setNumValue("CO2|CO2ModeDefault", index);
    d_ptr->co2Switch = on;
}

/**************************************************************************************************
 * ??????CO2?????????
 *************************************************************************************************/
bool CO2Param::getCO2Switch()
{
    return d_ptr->co2Switch;
}

/**************************************************************************************************
 * ??????CO2 Max value???
 *************************************************************************************************/
short CO2Param::getEtCO2MaxValue()
{
    return d_ptr->etco2MaxVal;
}

/**************************************************************************************************
 * ??????CO2 Min value???
 *************************************************************************************************/
short CO2Param::getEtCO2MinValue()
{
    return d_ptr->etco2MinVal;
}

void CO2Param::updateSubParamLimit(SubParamID id)
{
    if (id == SUB_PARAM_ETCO2)
    {
        d_ptr->trendWidget->updateLimit();
    }
}

void CO2Param::setRespApneaStimulation(bool sta)
{
    int co2ApneaStimulation = 0;
    currentConfig.getNumValue("ApneaStimulation|CO2", co2ApneaStimulation);
    O2ParamInterface *o2Param = O2ParamInterface::getO2ParamInterface();
    if (co2ApneaStimulation && o2Param && d_ptr->co2Switch)
    {
        o2Param->setVibrationReason(APNEASTIMULATION_REASON_ETCO2, sta);
    }
    else
    {
        o2Param->setVibrationReason(APNEASTIMULATION_REASON_ETCO2, false);
    }
}

void CO2Param::paramUpdateTimeout()
{
    d_ptr->etco2Value = InvData();
    d_ptr->fico2Value = InvData();

    if (d_ptr->trendWidget != NULL)
    {
        d_ptr->trendWidget->setEtCO2Value(d_ptr->etco2Value);
        d_ptr->trendWidget->setFiCO2Value(d_ptr->fico2Value);
    }
}

void CO2Param::onPaletteChanged(ParamID id)
{
    if (id != PARAM_CO2 || !systemManager.isSupport(CONFIG_CO2))
    {
        return;
    }
    QPalette pal = colorManager.getPalette(paramInfo.getParamName(PARAM_CO2));
    d_ptr->trendWidget->updatePalette(pal);
    d_ptr->waveWidget->updatePalette(pal);
}

/**************************************************************************************************
 * ?????????
 *************************************************************************************************/
CO2Param::CO2Param()
        : Param(PARAM_CO2),
          d_ptr(new CO2ParamPrivate(this))
{
    int t = UNIT_PERCENT;
    systemConfig.getNumValue("Unit|CO2Unit", t);

    QString path = "AlarmSource|";
    path += paramInfo.getSubParamName(SUB_PARAM_ETCO2);
    path += "|";
    if (UNIT_PERCENT == t)
    {
        path += Unit::getSymbol(UNIT_PERCENT);
    }
    else if (UNIT_MMHG == t)
    {
        path += Unit::getSymbol(UNIT_MMHG);
    }
    else if (UNIT_KPA == t)
    {
        path += Unit::getSymbol(UNIT_KPA);
    }
    path += "|";
    QString lowPath = path + "Low";
    QString highPath = path + "High";
    currentConfig.getNumAttr(lowPath, "Min", d_ptr->etco2MinVal);
    currentConfig.getNumAttr(highPath, "Max", d_ptr->etco2MaxVal);

    // ?????????????????????????????????????????????????????????????????????????????????
    if (d_ptr->provider)
    {
        d_ptr->provider->setWorkMode(C02_WORK_SLEEP);
    }
}

/**************************************************************************************************
 * ?????????
 *************************************************************************************************/
CO2Param::~CO2Param()
{
    delete d_ptr;
}

bool CO2Param::getStartCal()
{
    return d_ptr->startCal;
}
