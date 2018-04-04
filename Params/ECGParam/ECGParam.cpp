#include "ECGParam.h"
#include "ECGDupParam.h"
#include "BaseDefine.h"
#include "IConfig.h"
#include "ECGWaveWidget.h"
#include "OxyCRGHRWidget.h"
#include "ECGPVCSTrendWidget.h"
#include "ECGSTTrendWidget.h"
#include "ECGMenu.h"
#include "Debug.h"
#include "WindowManager.h"
#include "WaveWidgetSelectMenu.h"
#include "ECGProviderIFace.h"
#include "WaveformCache.h"
#include "SystemManager.h"
#include "SoftKeyManager.h"
#include "MonitorSoftkeyAction.h"
#include "SPO2Param.h"
#include "Provider.h"
#include "SystemStatusBarWidget.h"
#include "ECGAlarm.h"
#include "QApplication"
#include "ComboListPopup.h"
#include "SummaryStorageManager.h"
#include "TimeManager.h"
#include <QTimer>
#include "Debug.h"
#include <QTimer>
#include "ECG12LDataStorage.h"
#include "PrintManager.h"
#include "ECG12LeadManager.h"
#include "ErrorLogItem.h"
#include "ErrorLog.h"

#define ECG_TIMER_INTERVAL (100)
#define GET_DIA_DATA_PERIOD (12000)
#define DISABLE_DIA_SOFTKEY_PERIOD (900)//1s,定时有差异，使用900ms

unsigned ECGParam::selfTestResult = 0;
ECGParam *ECGParam::_selfObj = NULL;

/**************************************************************************************************
 * 获取禁用的波形控件。
 *************************************************************************************************/
void ECGParam::_getDisabledWaveforms(QStringList &waveforms)
{
    waveforms.clear();

    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[(ECGLead)i] == NULL)
        {
            return;
        }
    }

    if (_curLeadMode == ECG_LEAD_MODE_3)
    {
        ECGLead caclLead = getCalcLead();
        for (int i = ECG_LEAD_I; i <= ECG_LEAD_III; ++i)
        {
            if (i != caclLead)
            {
                waveforms.append(_waveWidget[i]->name());
            }
        }

        waveforms.append(_waveWidget[ECG_LEAD_AVR]->name());
        waveforms.append(_waveWidget[ECG_LEAD_AVL]->name());
        waveforms.append(_waveWidget[ECG_LEAD_AVF]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V1]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V2]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V3]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V4]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V5]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V6]->name());
    }
    else if (_curLeadMode == ECG_LEAD_MODE_5)
    {
        waveforms.append(_waveWidget[ECG_LEAD_V2]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V3]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V4]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V5]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V6]->name());
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void ECGParam::initParam(void)
{
    soundManager.setSoundVolume(SOUND_VOLUME_CHANNEL_HEART_BEAT, (SoundVolume)getQRSToneVolume());

    if (_provider == NULL)
    {
        return;
    }
}

/**************************************************************************************************
 * 处理DEMO时的波形数据。
 *************************************************************************************************/
void ECGParam::handDemoWaveform(WaveformID id, short data)
{
    if (NULL != _waveWidget[id])
    {
        _waveWidget[id]->addWaveformData(data);
    }
    waveformCache.addData((WaveformID)id, data);
}

/**************************************************************************************************
 * 产生DEMO数据。
 *************************************************************************************************/
void ECGParam::handDemoTrendData(void)
{
    int hrValue = qrand() % 10 + 60;
    ecgDupParam.updateHR(hrValue);

    int pvcs = qrand() % 30 + 30;
    updatePVCS(pvcs);

    int st;
    for (int i = ECG_ST_I;i<ECG_ST_NR;i++)
        {
        st = qrand() % 10 - 5;
        updateST((ECGST)i,st);
    }

}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short ECGParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
        case SUB_PARAM_HR_PR:
            return getHR();

        case SUB_PARAM_ECG_PVCS:
            return getPVCS();

        case SUB_PARAM_ST_I:
            return getST(ECG_ST_I);

        case SUB_PARAM_ST_II:
            return getST(ECG_ST_II);

        case SUB_PARAM_ST_III:
            return getST(ECG_ST_III);

        case SUB_PARAM_ST_aVR:
            return getST(ECG_ST_aVR);

        case SUB_PARAM_ST_aVL:
            return getST(ECG_ST_aVL);

        case SUB_PARAM_ST_aVF:
            return getST(ECG_ST_aVF);

        case SUB_PARAM_ST_V1:
            return getST(ECG_ST_V1);

        case SUB_PARAM_ST_V2:
            return getST(ECG_ST_V2);

        case SUB_PARAM_ST_V3:
            return getST(ECG_ST_V3);

        case SUB_PARAM_ST_V4:
            return getST(ECG_ST_V4);

        case SUB_PARAM_ST_V5:
            return getST(ECG_ST_V5);

        case SUB_PARAM_ST_V6:
            return getST(ECG_ST_V6);

        default:
            return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void ECGParam::showSubParamValue()
{
    if (_pvcsTrendWidget != NULL)
    {
        _pvcsTrendWidget->showValue();
    }
    if (_ecgSTTrendWidget != NULL)
    {
        _ecgSTTrendWidget->showValue();
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType ECGParam::getCurrentUnit(SubParamID id)
{
    switch (id)
    {
        case SUB_PARAM_HR_PR:
        case SUB_PARAM_ECG_PVCS:
            return UNIT_BPM;

        case SUB_PARAM_ST_I:
        case SUB_PARAM_ST_II:
        case SUB_PARAM_ST_III:
        case SUB_PARAM_ST_aVR:
        case SUB_PARAM_ST_aVL:
        case SUB_PARAM_ST_aVF:
        case SUB_PARAM_ST_V1:
        case SUB_PARAM_ST_V2:
        case SUB_PARAM_ST_V3:
        case SUB_PARAM_ST_V4:
        case SUB_PARAM_ST_V5:
        case SUB_PARAM_ST_V6:
            return UNIT_MV;

        default:
            return UNIT_BPM;
    }
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void ECGParam::setProvider(ECGProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }

    _provider = provider;

    // 初始化相关参数。
    // 导联模式。
    _provider->setLeadSystem(_curLeadMode);
    _provider->getLeadCabelType();

    // 计算导联。
    _provider->setCalcLead(getCalcLead());

    // 设置病人类型
    _provider->setPatientType(getPatientType());

    // 设置带宽。
    _provider->setBandwidth((ECGBandwidth)getBandwidth());

    // set fitler mode
    _provider->setFilterMode((ECGFilterMode)getFilterMode());

    // 设置起搏检测。
    _provider->enablePacermaker(getPacermaker());

    // 设置工频滤波。
    _provider->setNotchFilter(getNotchFilter());

    // enable data sync
    _provider->enableDataSyncCtrl(true);

    int p05, n05;
    _provider->get05mV(p05, n05);

    // 设置波形控件的采样率。
    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setDataRate(_provider->getWaveformSample());
        _waveWidget[i]->set05MV(p05, n05);
        QString tile = _waveWidget[i]->getTitle();
        // 注册波形缓存。
        waveformCache.registerSource((WaveformID)i, _provider->getWaveformSample(), n05, p05,
                tile, _provider->getBaseLine());
    }

    //register ECG calculation lead
    QString diagCalcTitle("CALC.ECG");
    waveformCache.registerSource(WAVE_ECG_CALC, _provider->getWaveformSample(), n05, p05,
                                 diagCalcTitle, _provider->getBaseLine());

    //申请完波形缓存后重新设置波形速率，因为进入监控页面波形是以250移动
    _provider->setWaveformSample(ECG_WAVE_RATE_250);
    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setDataRate(_provider->getWaveformSample());
    }
	_waveOxyCRGWidget->setDataRate(_provider->getWaveformSample());

//    <Gain>1</Gain>
    // todo：其他设置。
}

/**************************************************************************************************
 * 设置服务模式升级数据提供对象。
 *************************************************************************************************/
void ECGParam::setServiceProvider(ECGProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;
}


/**************************************************************************************************
 * 设置PVCS界面对象。
 *************************************************************************************************/
void ECGParam::setECGPVCSTrendWidget(ECGPVCSTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _pvcsTrendWidget = trendWidget;
}

/**************************************************************************************************
 * 设置ST界面对象。
 *************************************************************************************************/
void ECGParam::setECGSTTrendWidget(ECGSTTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _ecgSTTrendWidget = trendWidget;
}

/**************************************************************************************************
 * 设置波形对象。
 *************************************************************************************************/
void ECGParam::setWaveWidget(ECGWaveWidget *waveWidget, ECGLead whichLead)
{
    if (NULL != waveWidget)
    {
        _waveWidget[whichLead] = waveWidget;
        _waveWidget[whichLead]->setSpeed(getSweepSpeed());
    }
}

void ECGParam::setOxyCRGWaveWidget(OxyCRGHRWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }
    _waveOxyCRGWidget = waveWidget;
}

/**************************************************************************************************
 * 更新波形数据。
 * 参数：
 *      waveform: 波形数据，包括所有导联导波形；
 *      leadoff:导联脱落
 *      ipaceMark：内部起搏器标记；
 *      epaceMark：外部起搏标记；
 *      rMark： R波标记。
 *************************************************************************************************/
void ECGParam::updateWaveform(int waveform[], bool *leadoff, bool ipaceMark, bool /*epaceMark*/, bool rMark)
{
    int displaymode = ecgParam.getDisplayMode();
    int rate = _provider->getWaveformSample();
    UserFaceType faceType = windowManager.getUFaceType();

    _waveDataInvalid = !_waveDataInvalid;

    // 心跳音
    if (rMark)
    {
        soundManager.heartBeat();
        ecgDupParam.updateHRBeatIcon();
    }

    int flag = 0;
    if (ipaceMark)
    {
        flag |= ECG_INTERNAL_FLAG_BIT;
    }

    int tmpFalg = flag;
    int bandwidth = (ecgParam.getDisplayBandWidth() << 4);
    for (int i = ECG_LEAD_I; i <= ECG_LEAD_V6; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }

        flag = tmpFalg;

        //12导模式下需要内部起搏标记(数据无效时不标记)
        if ((faceType == UFACE_MONITOR_12LEAD) && ipaceMark && (!leadoff[i]))
        {
            flag |= ECG_INTERNAL_FLAG_BIT;
        }

        // 无效数据位
        if (leadoff[i])
        {
            flag |= INVALID_WAVE_FALG_BIT;
            flag &= ~ECG_INTERNAL_FLAG_BIT;
//            flag &= ~ECG_RWAVE_MARK_FLAG_BIT;
        }
        else
        {
            flag &= ~INVALID_WAVE_FALG_BIT;
        }

        // can't display wave set invalid flag
        switch (ecgParam.getLeadMode())
        {
            case ECG_LEAD_MODE_3:
                if (i > ECG_LEAD_III && i <= ECG_LEAD_V6)
                {
                    flag |= INVALID_WAVE_FALG_BIT;
                }
                break;

            case ECG_LEAD_MODE_5:
                if (i > ECG_LEAD_V1 && i <= ECG_LEAD_V6)
                {
                    flag |= INVALID_WAVE_FALG_BIT;
                }
                break;

            default:
                break;
        }

#if 1
        //诊断模式和12L模式的全部导联
        if ((faceType == UFACE_MONITOR_12LEAD) || (WAVE_SAMPLE_RATE_500 == rate && i != ECG_LEAD_V6 +1))
        {
            flag &= ~ECG_WAVE_RATE_250_BIT;
        }
        else
        {
            flag |= ECG_WAVE_RATE_250_BIT;
        }
#else
        if(WAVE_SAMPLE_RATE_250 == rate)
        {
            flag |= ECG_WAVE_RATE_250_BIT;
        }
        else
        {
            flag &= ~ECG_WAVE_RATE_250_BIT;
        }
#endif

//        if (flag & (ECG_EXTERNAL_DOT_FLAG_BIT | ECG_EXTERNAL_SOLD_FLAG_BIT))
        if (ECG_EXTERNAL_SOLD_FLAG_BIT)
        {
//            flag &= ~ECG_EXTERNAL_DOT_FLAG_BIT;
            flag &= ~ECG_EXTERNAL_SOLD_FLAG_BIT;
        }
        //当前带宽
        flag |= bandwidth;
        if(i == ECG_LEAD_V6 + 1) //TE3 CalCulation lead data, not MFC
        {
            //store the TE3 Calculation lead data
            waveformCache.addData(WAVE_ECG_CALC, ((flag & 0xFFFF) << 16) | (waveform[i] & 0xFFFF));
            break;
        }
        else
        {
            waveformCache.addData((WaveformID)i, ((flag & 0xFFFF) << 16) | (waveform[i] & 0xFFFF));
        }

        static int flagUnsaved[ECG_LEAD_NR] = {0};
        //在普通模式中，在监控模式下所有的波形数据要扔掉一半，以250显示
        if(ECG_DISPLAY_NORMAL == displaymode)
        {
            if (_waveDataInvalid)
            {
                flagUnsaved[i] = flag;
                continue;
            }
        }

        int norfalg = flag | flagUnsaved[i];
//        if (norfalg & (ECG_EXTERNAL_DOT_FLAG_BIT | ECG_EXTERNAL_SOLD_FLAG_BIT))
        if (norfalg & ECG_EXTERNAL_SOLD_FLAG_BIT)
        {
//            norfalg &= ~ECG_EXTERNAL_DOT_FLAG_BIT;
            norfalg &= ~ECG_EXTERNAL_SOLD_FLAG_BIT;
        }

        if (!(i == WAVE_ECG_aVR && faceType == UFACE_MONITOR_12LEAD &&
              ecgParam.get12LDisplayFormat() == DISPLAY_12LEAD_CABRELA))
        {
            _waveWidget[i]->addWaveformData(waveform[i], norfalg & 0xFFFF);
        }
        else
        {
            _waveWidget[i]->addWaveformData(-waveform[i], norfalg & 0xFFFF);
        }

        flagUnsaved[i] = 0;
    }
}

/**************************************************************************************************
 * 更新HR数值。
 *************************************************************************************************/
void ECGParam::updateHR(short hr)
{
    if (_hrValue == hr)
    {
        return;
    }

    _hrValue = hr;
    ecgDupParam.updateHR(hr);
}

/***************************************************************************************************
 * get hr
 **************************************************************************************************/
short ECGParam::getHR() const
{
    return _hrValue;
}

/**************************************************************************************************
 * 更新PVCS数值。
 *************************************************************************************************/
void ECGParam::updatePVCS(short pvcs)
{
    _pvcsValue = pvcs;
    if (_pvcsTrendWidget != NULL)
    {
        _pvcsTrendWidget->setPVCSValue(_pvcsValue);
    }
}

/***************************************************************************************************
 * get PVCS
 **************************************************************************************************/
short ECGParam::getPVCS() const
{
    return _pvcsValue;
}

/**************************************************************************************************
 * 更新ST数值。
 *************************************************************************************************/
void ECGParam::updateST(ECGST lead,short st)
{
    _stValue[lead] = st;
    if (_ecgSTTrendWidget != NULL)
    {
        _ecgSTTrendWidget->setSTValue(lead,st);
    }
}

/***************************************************************************************************
 * get ST
 **************************************************************************************************/
short ECGParam::getST(ECGST lead) const
{
    return _stValue[lead];
}

/**************************************************************************************************
 * 更新VFVT数值。
 *************************************************************************************************/
void ECGParam::updateVFVT(bool onoff)
{
    ecgDupParam.updateVFVT(onoff);
}

/**************************************************************************************************
 * 更新导联脱落状态。
 *************************************************************************************************/
void ECGParam::setLeadOff(ECGLead lead, bool status)
{
    static bool checkFirstLeadOn = true;

    if (_leadOff[lead] != status)
    {
        _leadOff[lead] = status;
//        updateECGNotifyMesg(lead);
    }

    if(checkFirstLeadOn && (0 == _leadOff[lead]) && _calcLead == lead)
    {
        QTimer::singleShot(6000, this, SLOT(presentRhythm()));
        checkFirstLeadOn = false;
    }

    if (!_isEverLeadOn[lead] && !status)
    {
        QString everLeadOnStr = "PrimaryCfg|ECG|EverLeadOn|";
        everLeadOnStr += ECGSymbol::convert(lead, ECG_CONVENTION_AAMI);
        switch (_curLeadMode)
        {
            case ECG_LEAD_MODE_3:
                if (lead > ECG_LEAD_III)
                {
                    return;
                }
                break;

            case ECG_LEAD_MODE_5:
                if (lead > ECG_LEAD_V1)
                {
                    return;
                }
                break;

            default:
                if (lead > ECG_LEAD_V6)
                {
                    return;
                }
                break;
        }

        _isEverLeadOn[lead] = true;
        systemConfig.setNumValue(everLeadOnStr, true);
    }
}

/**************************************************************************************************
 * 设置是否过载
 *************************************************************************************************/
void ECGParam::setOverLoad(bool flag)
{
    static bool overLoadFlag = false;

    if (overLoadFlag != flag)
    {
        ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_OVERLOAD, flag);
        overLoadFlag = flag;
    }
}

/**************************************************************************************************
 * update ecg notify message
 *************************************************************************************************/
void ECGParam::updateECGNotifyMesg(ECGLead lead, bool isAlarm)
{
    if (lead >= ECG_LEAD_NR)
    {
        return;
    }

    ECGWaveNotify mesg = ECG_WAVE_NOTIFY_NORMAL;

    if (isConnected())
    {
        if (1 == _leadOff[lead])
        {
            mesg = ECG_WAVE_NOTIFY_LEAD_OFF;
        }

        if (isAlarm)
        {
            if (lead == _calcLead)
            {
                mesg = ECG_WAVE_NOTIFY_CHECK_PATIENT;
            }
        }
    }

    if (_waveWidget[lead])
    {
        _waveWidget[lead]->setNotifyMesg(mesg);
    }
}

/**************************************************************************************************
 * 获取导联脱落状态。
 *************************************************************************************************/
char ECGParam::doesLeadOff(int lead)
{
    if (lead >= ECG_LEAD_NR)
    {
        return 2;
    }

    //never receive ecg module lead status
    if (2 == _leadOff[lead])
    {
        return _leadOff[lead];
    }

    return _leadOff[lead] && isConnected();
}

/**************************************************************************************************
 * 是否连接。
 *************************************************************************************************/
bool ECGParam::isConnected()
{
    if (_provider == NULL)
    {
        return false;
    }
    Provider *provider = dynamic_cast<Provider*>(_provider);
    if (NULL == provider)
    {
        return false;
    }

    return provider->connected();
}

/**************************************************************************************************
 * 进入12导模式。
 *************************************************************************************************/
void ECGParam::enter12Lead(void)
{
    setDisplayMode(ECG_DISPLAY_12_LEAD_FULL);
}

/**************************************************************************************************
 * 退出12导模式。
 *************************************************************************************************/
void ECGParam::exit12Lead(void)
{
    setDisplayMode(ECG_DISPLAY_NORMAL);
}

/**************************************************************************************************
 * 设置OneShot报警。
 *************************************************************************************************/
void ECGParam::setOneShotAlarm(ECGOneShotType t, bool f)
{
    ecgOneShotAlarm.setOneShotAlarm(t, f);
}

/**************************************************************************************************
 * 超限报警通知。
 *************************************************************************************************/
void ECGParam::noticeLimitAlarm(int id, bool isAlarm)
{
    switch (id)
    {
        case SUB_PARAM_ECG_PVCS:
            if (_pvcsTrendWidget != NULL)
            {
                _pvcsTrendWidget->isAlarm(isAlarm);
            }
            break;

        case SUB_PARAM_ST_I:
        case SUB_PARAM_ST_II:
        case SUB_PARAM_ST_III:
        case SUB_PARAM_ST_aVR:
        case SUB_PARAM_ST_aVL:
        case SUB_PARAM_ST_aVF:
        case SUB_PARAM_ST_V1:
        case SUB_PARAM_ST_V2:
        case SUB_PARAM_ST_V3:
        case SUB_PARAM_ST_V4:
        case SUB_PARAM_ST_V5:
        case SUB_PARAM_ST_V6:
            if (_ecgSTTrendWidget != NULL)
            {
                _ecgSTTrendWidget->isAlarm((id - SUB_PARAM_ST_I),isAlarm);
            }
            break;

        default:
            break;
    }
}

/**************************************************************************************************
 * 获取支持的工作模式。
 *************************************************************************************************/
ECGLeadMode ECGParam::getAvailableMode(void)
{
    int value = 0;
    machineConfig.getNumValue("ECG12LEADEnable", value);
    if (0 == value)
    {
        return ECG_LEAD_MODE_5;
    }
    else
    {
        return ECG_LEAD_MODE_12;
    }
}

/**************************************************************************************************
 * 获取Topwaveform的名称。
 *************************************************************************************************/
const QString &ECGParam::getCalcLeadWaveformName(void)
{
    ECGLead lead = getCalcLead();
    if (_waveWidget[lead] != NULL)
    {
        return _waveWidget[lead]->name();
    }
    else
    {
        QString str;
        return str = "";
    }
}

/**************************************************************************************************
 * 获取导联对应的波形窗体的名称。
 *************************************************************************************************/
const QString &ECGParam::getWaveWidgetName(ECGLead lead)
{
    if ((ECG_LEAD_NR <= lead) || (ECG_LEAD_I > lead))
    {
        QString str;
        return str = "";
    }

    if (_waveWidget[lead] != NULL)
    {
        return _waveWidget[lead]->name();
    }
    else
    {
        QString str;
        return str = "";
    }
}

/**************************************************************************************************
 * 获取可得的导联集。
 *************************************************************************************************/
void ECGParam::getAvailableLeads(QList<ECGLead> &leads)
{
    leads.clear();
    leads.append(ECG_LEAD_I);
    leads.append(ECG_LEAD_II);
    leads.append(ECG_LEAD_III);

    if (_curLeadMode >= ECG_LEAD_MODE_5)
    {
        leads.append(ECG_LEAD_AVR);
        leads.append(ECG_LEAD_AVL);
        leads.append(ECG_LEAD_AVF);
        leads.append(ECG_LEAD_V1);
    }
    if (_curLeadMode >= ECG_LEAD_MODE_12)
    {
        leads.append(ECG_LEAD_V2);
        leads.append(ECG_LEAD_V3);
        leads.append(ECG_LEAD_V4);
        leads.append(ECG_LEAD_V5);
        leads.append(ECG_LEAD_V6);
    }

    // 剔除界面上显示心电导联波形。
    QList<int> waveforms;
    windowManager.getDisplayedWaveform(waveforms);
    for (int i = 0; i < waveforms.size(); i++)
    {
        switch ((WaveformID)waveforms[i])
        {
        case WAVE_ECG_I:
            if (getCalcLead() != ECG_LEAD_I)
            {
                leads.removeOne(ECG_LEAD_I);
            }
            break;

        case WAVE_ECG_II:
            if (getCalcLead() != ECG_LEAD_II)
            {
                leads.removeOne(ECG_LEAD_II);
            }

            break;
        case WAVE_ECG_III:
            if (getCalcLead() != ECG_LEAD_III)
            {
                leads.removeOne(ECG_LEAD_III);
            }
            break;

        case WAVE_ECG_aVR:
            if (getCalcLead() != ECG_LEAD_AVR)
            {
                leads.removeOne(ECG_LEAD_AVR);
            }
            break;

        case WAVE_ECG_aVL:
            if (getCalcLead() != ECG_LEAD_AVL)
                leads.removeOne(ECG_LEAD_AVL);
            break;
        case WAVE_ECG_aVF:
            if (getCalcLead() != ECG_LEAD_AVF)
            {
                leads.removeOne(ECG_LEAD_AVF);
            }
            break;

        case WAVE_ECG_V1:
            if (getCalcLead() != ECG_LEAD_V1)
                leads.removeOne(ECG_LEAD_V1);
            break;

        case WAVE_ECG_V2:
            if (getCalcLead() != ECG_LEAD_V2)
            {
                leads.removeOne(ECG_LEAD_V2);
            }
            break;

        case WAVE_ECG_V3:
            if (getCalcLead() != ECG_LEAD_V3)
            {
                leads.removeOne(ECG_LEAD_V3);
            }
            break;

        case WAVE_ECG_V4:
            if (getCalcLead() != ECG_LEAD_V4)
            {
                leads.removeOne(ECG_LEAD_V4);
            }
            break;

        case WAVE_ECG_V5:
            if (getCalcLead() != ECG_LEAD_V5)
            {
                leads.removeOne(ECG_LEAD_V5);
            }
            break;

        case WAVE_ECG_V6:
            if (getCalcLead() != ECG_LEAD_V6)
            {
                leads.removeOne(ECG_LEAD_V6);
            }
            break;

        default:
            break;
        }
    }

    if (ECG_LEAD_MODE_3 == _curLeadMode)
    {
        // 保留一道ecg波形
        QStringList waveList;
        windowManager.getDisplayedWaveform(waveList);
    }
}


/**************************************************************************************************
 * wave ID->Lead ID。
 *************************************************************************************************/
ECGLead ECGParam::waveIDToLeadID(WaveformID id)
{
    switch (id)
    {
        case WAVE_ECG_I:
            return ECG_LEAD_I;
        case WAVE_ECG_II:
            return ECG_LEAD_II;
        case WAVE_ECG_III:
            return ECG_LEAD_III;
        case WAVE_ECG_aVR:
            return ECG_LEAD_AVR;
        case WAVE_ECG_aVL:
            return ECG_LEAD_AVL;
        case WAVE_ECG_aVF:
            return ECG_LEAD_AVF;
        case WAVE_ECG_V1:
            return ECG_LEAD_V1;
        case WAVE_ECG_V2:
            return ECG_LEAD_V2;
        case WAVE_ECG_V3:
            return ECG_LEAD_V3;
        case WAVE_ECG_V4:
            return ECG_LEAD_V4;
        case WAVE_ECG_V5:
            return ECG_LEAD_V5;
        case WAVE_ECG_V6:
            return ECG_LEAD_V6;
        default:
            return ECG_LEAD_NR;
    }
}

/**************************************************************************************************
 * lead ID->wave ID。
 *************************************************************************************************/
WaveformID ECGParam::leadToWaveID(ECGLead lead)
{
    switch (lead)
    {
        case ECG_LEAD_I:
            return WAVE_ECG_I;
        case ECG_LEAD_II:
            return WAVE_ECG_II;
        case ECG_LEAD_III:
            return WAVE_ECG_III;
        case ECG_LEAD_AVR:
            return WAVE_ECG_aVR;
        case ECG_LEAD_AVL:
            return WAVE_ECG_aVL;
        case ECG_LEAD_AVF:
            return WAVE_ECG_aVF;
        case ECG_LEAD_V1:
            return WAVE_ECG_V1;
        case ECG_LEAD_V2:
            return WAVE_ECG_V2;
        case ECG_LEAD_V3:
            return WAVE_ECG_V3;
        case ECG_LEAD_V4:
            return WAVE_ECG_V4;
        case ECG_LEAD_V5:
            return WAVE_ECG_V5;
        case ECG_LEAD_V6:
            return WAVE_ECG_V6;
        default:
            return WAVE_ECG_II;
    }
}

/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void ECGParam::reset(void)
{
    if (_provider == NULL)
    {
        return;
    }

    _provider->setLeadSystem(_curLeadMode);
    _provider->getLeadCabelType();

    // 计算导联。
    _provider->setCalcLead(getCalcLead());

    // 设置病人类型
    _provider->setPatientType(getPatientType());

    // 设置带宽。
    _provider->setBandwidth((ECGBandwidth)getBandwidth());

    // 设置起搏检测。
    _provider->enablePacermaker(getPacermaker());

    // 设置工频滤波。
    _provider->setNotchFilter(getNotchFilter());

    int p05, n05;
    _provider->get05mV(p05, n05);
}

/***************************************************************************************************
 * handle ecg selftest result
 **************************************************************************************************/
void ECGParam::handleSelfTestResult()
{
    QString errStr("");
    bool flag = true;
    if (selfTestResult & MAJOR_SAMPLE_MODULE_INIT_FAILED)
    {
        errStr += "Major Collection Module Init Failed.\r\n";
        flag = false;
    }

    if (selfTestResult & MINOR_SAMPLE_MODULE_INIT_FAILED)
    {
        errStr += "Minor Collection Module Init Failed.\r\n";
        flag = false;
    }

    if (selfTestResult & PD_BLANK_TEST_FAILED)
    {
        errStr += "PD Blank Test Failed.\r\n";
        flag = false;
    }

    if (selfTestResult & PACE_SYNC_TEST_FAILED)
    {
        errStr += "Pace Sync Test Failed.\r\n";
        flag = false;
    }
    systemManager.setPoweronTestResult(TE3_MODULE_SELFTEST_RESULT,
            flag ? SELFTEST_SUCCESS : SELFTEST_FAILED);

    if (!errStr.isEmpty())
    {
        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("TE3 SelfTest Failed");
        item->setLog(errStr);
        item->setSubSystem(ErrorLogItem::SUB_SYS_TE3);
        item->setSystemState(ErrorLogItem::SYS_STAT_SELFTEST);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);
    }
}

/**************************************************************************************************
 * enable VF calc。
 *************************************************************************************************/
void ECGParam::enableVFCalc(bool enable)
{
    if (NULL != _provider)
    {
        _provider->enableVFCalcCtrl(enable);
    }
}

/**************************************************************************************************
 * 获取可得的波形控件集。
 *************************************************************************************************/
void ECGParam::getAvailableWaveforms(QStringList &waveforms,
        QStringList &waveformShowName,int)
{
    waveforms.clear();
    waveformShowName.clear();

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[(ECGLead)i] == NULL)
        {
            return;
        }
    }

    waveforms.append(_waveWidget[ECG_LEAD_I]->name());
    waveforms.append(_waveWidget[ECG_LEAD_II]->name());
    waveforms.append(_waveWidget[ECG_LEAD_III]->name());
    waveformShowName.append(_waveWidget[ECG_LEAD_I]->getTitle());
    waveformShowName.append(_waveWidget[ECG_LEAD_II]->getTitle());
    waveformShowName.append(_waveWidget[ECG_LEAD_III]->getTitle());

    if (_curLeadMode >= ECG_LEAD_MODE_5)
    {
        waveforms.append(_waveWidget[ECG_LEAD_AVR]->name());
        waveforms.append(_waveWidget[ECG_LEAD_AVL]->name());
        waveforms.append(_waveWidget[ECG_LEAD_AVF]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V1]->name());

        waveformShowName.append(_waveWidget[ECG_LEAD_AVR]->getTitle());
        waveformShowName.append(_waveWidget[ECG_LEAD_AVL]->getTitle());
        waveformShowName.append(_waveWidget[ECG_LEAD_AVF]->getTitle());
        waveformShowName.append(_waveWidget[ECG_LEAD_V1]->getTitle());
    }
    if (_curLeadMode >= ECG_LEAD_MODE_12)
    {
        waveforms.append(_waveWidget[ECG_LEAD_V2]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V3]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V4]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V5]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V6]->name());

        waveformShowName.append(_waveWidget[ECG_LEAD_V2]->getTitle());
        waveformShowName.append(_waveWidget[ECG_LEAD_V3]->getTitle());
        waveformShowName.append(_waveWidget[ECG_LEAD_V4]->getTitle());
        waveformShowName.append(_waveWidget[ECG_LEAD_V5]->getTitle());
        waveformShowName.append(_waveWidget[ECG_LEAD_V6]->getTitle());
    }
}

/**************************************************************************************************
 * 获取可得的趋势控件集。
 *************************************************************************************************/
void ECGParam::getTrendWindow(QStringList &trend)
{
    trend.clear();
    if (_pvcsTrendWidget != NULL)
    {
        trend.append(_pvcsTrendWidget->name());
    }
    if (_ecgSTTrendWidget != NULL)
    {
        trend.append(_ecgSTTrendWidget->name());
    }
}

/**************************************************************************************************
 * 获取导联模式。
 *************************************************************************************************/
void ECGParam::setLeadMode(ECGLeadMode newMode)
{
    if(newMode == _curLeadMode)
    {
        return;
    }

    waveWidgetSelectMenu.close();

    ECGLeadMode oldMode = _curLeadMode;

    // 将新模式保存到配置文件中。
    systemConfig.setNumValue("PrimaryCfg|ECG|LeadMode", (int)newMode);
    _curLeadMode = newMode;
    if (NULL != _provider)
    {
        _provider->setLeadSystem(newMode);
    }

    int defaultLead = ECG_LEAD_II;
    superConfig.getNumValue("ECG|DefaultECGLeadInMonitorMode", defaultLead);

    // 切换到正确的计算导联。
    ECGLead calcLead = getCalcLead();
    ECGLead newCaclLead = calcLead;
    if (oldMode == ECG_LEAD_MODE_3)      // 3导切5导或12导，计算导联不用更改。
    {
        if (newMode == ECG_LEAD_MODE_12)
        {

        }

        if (newMode == ECG_LEAD_MODE_5)
        {

        }
    }
    else if (oldMode == ECG_LEAD_MODE_5) // 5导切3导或12导。
    {
        if (newMode == ECG_LEAD_MODE_3)
        {
            if (calcLead > ECG_LEAD_III)
            {
                newCaclLead = ECG_LEAD_II;
            }
        }
        else if (newMode == ECG_LEAD_MODE_12)
        {

        }
    }
    else if (oldMode == ECG_LEAD_MODE_12)  // 12导切5到或3导。
    {
        if (newMode == ECG_LEAD_MODE_5)    // 切换5导。
        {
            if (calcLead > ECG_LEAD_V1)
            {
                newCaclLead = ECG_LEAD_II;
            }
        }
        else if (newMode == ECG_LEAD_MODE_3) // 切换3导。
        {
            if (calcLead > ECG_LEAD_III)
            {
                newCaclLead = ECG_LEAD_II;
            }
        }
    }

    if (calcLead != newCaclLead)
    {
        setCalcLead(newCaclLead);
        if (windowManager.getUFaceType() != UFACE_MONITOR_12LEAD)
        {
            windowManager.replaceWaveform(_waveWidget[calcLead]->name(), _waveWidget[newCaclLead]->name());
        }
        else
        {
             ecgDupParam.setECGTrendWidgetCalcName(_calcLead);
        }
    }

    if (windowManager.getUFaceType() != UFACE_MONITOR_12LEAD)
    {
        QStringList disabledWaveforms;
        _getDisabledWaveforms(disabledWaveforms);  // 不会被显示的波形集合。
        if (newMode == ECG_LEAD_MODE_3)
        {
            windowManager.changeToECGLead3(disabledWaveforms);

            QStringList waveName;
            windowManager.getDisplayedWaveform(waveName);
            for (int i = 0; i < waveName.size(); ++i)
            {
                setLeadMode3DisplayLead(waveName.at(i));
            }
        }
        else
        {
            windowManager.setExcludeWaveforms(disabledWaveforms);
        }
    }

}

/**************************************************************************************************
 * 获取导联模式。
 *************************************************************************************************/
ECGLeadMode ECGParam::getLeadMode(void) const
{
    return _curLeadMode;
}

/**************************************************************************************************
 * 设置显示模式。
 *************************************************************************************************/
void ECGParam::setDisplayMode(ECGDisplayMode mode, bool refresh)
{
    int band = 0;
    int filter = 0;
    ECGPaceMode ecgPaceMaker = getPacermaker();
    ECGPaceMode ecg12LPaceMaker = get12LPacermaker();

    if (mode == ECG_DISPLAY_NORMAL)
    {
        band = _chestFreqBand;
        filter = ECG_NOTCH_50_AND_60HZ;
    }
    else
    {
        //进入12L模式前，如果是在诊断时间范围內则先关闭诊断功能
        band = _12LeadFreqBand;

        superRunConfig.getNumValue("ECG12L|NotchFilter", filter);
    }

    if (NULL != _provider)
    {
        _provider->setBandwidth((ECGBandwidth)band);
        _provider->setNotchFilter((ECGNotchFilter)filter);
    }

    systemConfig.setNumValue("PrimaryCfg|ECG|DisplayMode", (int)mode);
    _displayMode = mode;

    // 布局界面。
    UserFaceType type = UFACE_MONITOR_STANDARD;
    if (mode == ECG_DISPLAY_NORMAL)
    {
        //设置波形采样率
        _provider->setWaveformSample(WAVE_SAMPLE_RATE_250);

        //退出12L界面前先还原起搏标记
        setPacermaker(ecgPaceMaker);
    }
    else if (mode == ECG_DISPLAY_12_LEAD_FULL)
    {
        type = UFACE_MONITOR_12LEAD;

        //设置波形采样率
        _provider->setWaveformSample(WAVE_SAMPLE_RATE_500);

        //进入12L界面前先设置起搏标记
        set12LPacermaker(ecg12LPaceMaker);
    }

    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        //更新波形速率
        _waveWidget[i]->setDataRate(_provider->getWaveformSample());
    }
    ecgMenu.refresh();

    if (!refresh)
    {
        return;
    }

    windowManager.setUFaceType(type);

    // 立即刷新界面，防止界面残留
    QApplication::processEvents(QEventLoop::ExcludeSocketNotifiers |
                                QEventLoop::ExcludeUserInputEvents);

    // 关闭下拉弹出框
    if (ComboListPopup::current())
    {
        ComboListPopup::current()->close();
    }

    // 清除上个模式留下的弹出框
    if (systemManager.isAcknownledgeSystemTestResult())
    {
        while (NULL != QApplication::activeModalWidget())
        {
            QApplication::activeModalWidget()->hide();
        }
    }

    //由12L界面退出至普通界面由于可能存在导联切换问题，因此需要对波形的显示做相关处理
    if (mode != ECG_DISPLAY_12_LEAD_FULL)
    {
        ECGLead calLead = getCalcLead();
        QStringList currentWaveforms;
        windowManager.getCurrentWaveforms(currentWaveforms);
        if ((!currentWaveforms.isEmpty()) && (currentWaveforms.at(0) != _waveWidget[calLead]->name()))
        {
            windowManager.replaceWaveform(currentWaveforms.at(0), _waveWidget[calLead]->name());
        }

        QStringList disabledWaveforms;
        _getDisabledWaveforms(disabledWaveforms);  // 不会被显示的波形集合。
        if (_curLeadMode == ECG_LEAD_MODE_3)
        {
            windowManager.changeToECGLead3(disabledWaveforms);

            QStringList waveName;
            windowManager.getDisplayedWaveform(waveName);
            for (int i = 0; i < waveName.size(); ++i)
            {
                setLeadMode3DisplayLead(waveName.at(i));
            }
        }
        else
        {
            windowManager.setExcludeWaveforms(disabledWaveforms);
        }
    }

    //如果12L还在进行文件传输则重新弹出进度条
    if ((mode == ECG_DISPLAY_12_LEAD_FULL) && (ecg12LeadManager.isTransfer()))
    {
        if (EMAIL_SELECT == ecg12LeadManager.getTransferType())
        {
            unsigned char barBalue = (unsigned char)(ecg12LeadManager._exportWifiWidget->getBarValue());
            ecg12LeadManager._exportWifiWidget->setVisible(true);
            ecg12LeadManager._exportWifiWidget->setBarValue(barBalue);
        }
        else
        {
            unsigned char barBalue = (unsigned char)(ecg12LeadManager._exportUsbWidget->getBarValue());
            ecg12LeadManager._exportUsbWidget->setVisible(true);
            ecg12LeadManager._exportUsbWidget->setBarValue(barBalue);
        }
    }
}

/**************************************************************************************************
 * 获取显示模式。
 *************************************************************************************************/
ECGDisplayMode ECGParam::getDisplayMode(void)
{
    return _displayMode;
}

/**************************************************************************************************
 * 根据输入的波形名称设置计算导联。
 *************************************************************************************************/
void ECGParam::setCalcLead(const QString &leadWaveform)
{
    for (int i = 0; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        if (_waveWidget[i]->name() == leadWaveform)
        {
            setCalcLead((ECGLead)i);
            break;
        }
    }
}

/**************************************************************************************************
 * 设置计算导联。
 *************************************************************************************************/
void ECGParam::setCalcLead(ECGLead lead)
{
    QList<ECGLead> leads;
    leads.clear();
    leads.append(ECG_LEAD_I);
    leads.append(ECG_LEAD_II);
    leads.append(ECG_LEAD_III);

    if (_curLeadMode >= ECG_LEAD_MODE_5)
    {
        leads.append(ECG_LEAD_AVR);
        leads.append(ECG_LEAD_AVL);
        leads.append(ECG_LEAD_AVF);
        leads.append(ECG_LEAD_V1);
    }
    if ( _curLeadMode >= ECG_LEAD_MODE_12)
    {
        leads.append(ECG_LEAD_V2);
        leads.append(ECG_LEAD_V3);
        leads.append(ECG_LEAD_V4);
        leads.append(ECG_LEAD_V5);
        leads.append(ECG_LEAD_V6);
    }

    if (windowManager.getUFaceType() != UFACE_MONITOR_12LEAD)
    {
        //切换计算导联的时候先退出诊断在切换，原因是因为诊断会改变波形采样和速率
        restoreDiagBandwidth();
    }

    ECGLead preCalcLead = getCalcLead();
    leads.clear();
    leads.append(ECG_LEAD_I);
    leads.append(ECG_LEAD_II);
    leads.append(ECG_LEAD_III);

    if (-1 == leads.indexOf(lead))
    {
        lead = ECG_LEAD_II;
    }

    if (preCalcLead == lead)
    {
        return;
    }

    // 将新的计算导联保存道配置文件。
    superRunConfig.setNumValue("ECG|DefaultECGLeadInMonitorMode", (int)lead);
    _calcLead = lead;
    if (NULL != _provider)
    {
        _provider->setCalcLead(lead);
    }

    //need to refresh to hide or show the Diag.ECG soft key
    softkeyManager.refresh();

    emit calcLeadChanged();
}

/**************************************************************************************************
 * 3导联模式，设置显示导联。
 *************************************************************************************************/
void ECGParam::setLeadMode3DisplayLead(ECGLead lead)
{
    if (lead > ECG_LEAD_III)
    {
        return;
    }

    if (NULL != _provider)
    {
        _provider->setCalcLead(lead);
    }
}

/**************************************************************************************************
 * 3导联模式，设置显示导联。
 *************************************************************************************************/
void ECGParam::setLeadMode3DisplayLead(const QString &leadWaveform)
{
    if (_curLeadMode != ECG_LEAD_MODE_3)
    {
        return;
    }

    QStringList waveNameList;
    waveNameList << _waveWidget[ECG_LEAD_I]->name()
                 << _waveWidget[ECG_LEAD_II]->name()
                 << _waveWidget[ECG_LEAD_III]->name();
    int index = waveNameList.indexOf(leadWaveform);
    if (-1 != index)
    {
        setLeadMode3DisplayLead((ECGLead)index);
    }
}

/**************************************************************************************************
 * 获取计算导联。
 *************************************************************************************************/
ECGLead ECGParam::getCalcLead(void)
{
    return _calcLead;
}

/**************************************************************************************************
 * 轮转设置计算导联。
 *************************************************************************************************/
void ECGParam::autoSetCalcLead(void)
{
    // 获取支持的导联。
    QList<ECGLead> leads;
    getAvailableLeads(leads);

    if (leads.isEmpty())
    {
        return;
    }

    // 当前的计算导联。
    ECGLead calcLead = getCalcLead();

    int index = 0;
    for (index = 0; index < leads.size(); index++)
    {
        if (leads[index] == calcLead)
        {
            break;
        }
    }

    if (index >= leads.size())  // 没找到则置为0。
    {
        return;
    }
    else
    {
        index++;
        index = (index >= leads.size()) ? 0 : index;
    }

    if (leads[index] == calcLead)
    {
        return;
    }

    setCalcLead(leads[index]);
    if (NULL != _waveWidget[calcLead] && NULL != _waveWidget[leads[index]])
    {
        windowManager.replaceWaveform(_waveWidget[calcLead]->name(),
                                      _waveWidget[leads[index]]->name(), false);
    }
    ecgMenu.refresh();

    // 关闭下拉弹出框
    if (ComboListPopup::current())
    {
        ComboListPopup::current()->close();
    }

    // 关闭波形菜单
    waveWidgetSelectMenu.close();
}

/**************************************************************************************************
 * 设置病人类型。
 *************************************************************************************************/
void ECGParam::setPatientType(unsigned char type)
{
    if (NULL != _provider)
    {
        _provider->setPatientType(type);
    }
}

/**************************************************************************************************
 * 获取病人类型。
 *************************************************************************************************/
unsigned char ECGParam::getPatientType(void)
{
    int type = 3;
    superRunConfig.getNumValue("General|DefaultPatientType", type);
    return (unsigned char)type;
}
/**************************************************************************************************
 * 设置带宽。
 *************************************************************************************************/
void ECGParam::setBandwidth(int band)
{
    if (_curLeadMode != ECG_LEAD_MODE_12)
    {
        superRunConfig.setNumValue("ECG|ChestLeadsECGBandwidth", (int)band);
        _chestFreqBand = (ECGBandwidth) band;
    }
    else
    {
        if (ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
        {
            superRunConfig.setNumValue("ECG12L|ECG12LeadBandwidth", (int)band);
            band += ECG_BANDWIDTH_0525_40HZ;
            _12LeadFreqBand = (ECGBandwidth) band;
        }
        else
        {
            superRunConfig.setNumValue("ECG|ChestLeadsECGBandwidth", (int)band);
            _chestFreqBand = (ECGBandwidth) band;
        }
    }

    if (NULL != _provider)
    {
        _provider->setBandwidth((ECGBandwidth)band);
    }

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setBandWidth((ECGBandwidth)band);
    }
}

/**************************************************************************************************
 * 设置诊断带宽。
 *************************************************************************************************/
void ECGParam::setDiagBandwidth()
{
    if (_isDisableDiaSoftKey)
    {
        return;
    }

    _isDisableDiaSoftKey = true;
    if (NULL != _provider)
    {
        _provider->setBandwidth(ECG_BANDWIDTH_0525_40HZ);

        int filter = 0;
        superRunConfig.getNumValue("ECG12L|NotchFilter", filter);
        _provider->setNotchFilter((ECGNotchFilter)filter);

        //重新设置波形速率
        _provider->setWaveformSample(WAVE_SAMPLE_RATE_500);
        for (int i = 0; i < ECG_LEAD_NR; ++i)
        {
            if (_waveWidget[i] == NULL)
            {
                continue;
            }
            _waveWidget[i]->setDataRate(_provider->getWaveformSample());
        }

        unsigned ts = timeManager.getCurTime();
        summaryStorageManager.addDiagnosticECG(ts, ECG_BANDWIDTH_0525_40HZ);
        _lastDiagModeTimestamp = ts;
    }

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setBandWidth(ECG_BANDWIDTH_0525_40HZ);
    }
}

/**************************************************************************************************
 * 设置诊断带宽, if restore after diagnostic complete, no need to stop trigger print
 *************************************************************************************************/
void ECGParam::restoreDiagBandwidth(int isCompleted)
{
    _isDisableDiaSoftKey = false;
    int band = 0;
    band = _chestFreqBand;

    if (NULL != _provider)
    {
        _provider->setBandwidth((ECGBandwidth)band);
        _provider->setNotchFilter(getNotchFilter());

        //重新设置波形速率
        _provider->setWaveformSample(WAVE_SAMPLE_RATE_250);
        for (int i = 0; i < ECG_LEAD_NR; ++i)
        {
            if (_waveWidget[i] == NULL)
            {
                continue;
            }
            _waveWidget[i]->setDataRate(_provider->getWaveformSample());
        }

    }

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setBandWidth((ECGBandwidth)band);
    }

    // 停止打印
    if (printManager.isPrinting() &&
            printManager.getCurPrintType() == PRINT_TYPE_TRIGGER_DIAG_ECG && !isCompleted)
    {
        printManager.abort();
    }
}

/***************************************************************************************************
 * get the bandwidth of the calc lead
 **************************************************************************************************/
ECGBandwidth ECGParam::getCalcLeadBandWidth()
{
    return _chestFreqBand;
}

/**************************************************************************************************
 * 获取带宽。
 *************************************************************************************************/
ECGBandwidth ECGParam::getBandwidth(void)
{
    int band = 0;

    if(ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
    {
        band = _12LeadFreqBand;
    }
    else
    {
        band = _chestFreqBand;
    }

    return (ECGBandwidth)band;
}

/**************************************************************************************************
 * 获取带宽。
 *************************************************************************************************/
ECGBandwidth ECGParam::getMFCBandwidth(void)
{
    int band = 0;
    superRunConfig.getNumValue("ECG|PadsECGBandwidth", band);

    return (ECGBandwidth)band;
}

/**************************************************************************************************
 * 获取显示带宽。
 *************************************************************************************************/
ECGBandwidth ECGParam::getDisplayBandWidth(void)
{
    int band = 0;
    band = _chestFreqBand;
    if(_displayMode == ECG_DISPLAY_12_LEAD_FULL)
    {
        band = _12LeadFreqBand;
    }
    else
    {
        band = getDiagBandwidth();
    }

    return (ECGBandwidth)band;
}

/**************************************************************************************************
 * set filter mode
 *************************************************************************************************/
void ECGParam::setFilterMode(int mode)
{
    if(mode == _filterMode)
        return;
    _filterMode = (ECGFilterMode)mode;
    if (NULL != _provider)
    {
        _provider->setFilterMode(_filterMode);
    }
}

/**************************************************************************************************
 * get filter mode
 *************************************************************************************************/
ECGFilterMode ECGParam::getFilterMode() const
{
    return _filterMode;
}

/**************************************************************************************************
 * 获取计算导联带宽字符串
 *************************************************************************************************/
QString ECGParam::getCalBandWidthStr(void)
{
    QString band = "";

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        if (i == ecgParam.getCalcLead())
        {
            band = _waveWidget[i]->getBandWidthStr();
        }
    }

    return band;
}

/**************************************************************************************************
 * 设置起搏标记。
 *************************************************************************************************/
void ECGParam::setPacermaker(ECGPaceMode onoff)
{
    systemConfig.setNumValue("PrimaryCfg|ECG|PacerMaker", (int)onoff);

    if (ECG_DISPLAY_12_LEAD_FULL != ecgParam.getDisplayMode())
    {
        if (NULL != _provider)
        {
            _provider->enablePacermaker(onoff);
        }
        sysStatusBar.changeIcon(SYSTEM_ICON_LABEL_PACER, (int)onoff);
    }

    return ;
}

/**************************************************************************************************
 * 获取起搏标记。
 *************************************************************************************************/
ECGPaceMode ECGParam::getPacermaker(void)
{
    int onoff = 0;
    systemConfig.getNumValue("PrimaryCfg|ECG|PacerMaker", onoff);
    return (ECGPaceMode)onoff;
}

/**************************************************************************************************
 * 设置12L起搏标记。
 *************************************************************************************************/
void ECGParam::set12LPacermaker(ECGPaceMode onoff)
{
    systemConfig.setNumValue("PrimaryCfg|ECG|ECG12LeadPacerMaker", (int)onoff);
    _12LeadPacerMarker = onoff;

    if (ECG_DISPLAY_12_LEAD_FULL == ecgParam.getDisplayMode())
    {
        if (NULL != _provider)
        {
            _provider->enablePacermaker(onoff);
        }
        sysStatusBar.changeIcon(SYSTEM_ICON_LABEL_PACER, (int)onoff);
    }

    return;
}

/**************************************************************************************************
 * 获取12L起搏标记。
 *************************************************************************************************/
ECGPaceMode ECGParam::get12LPacermaker(void)
{
    return _12LeadPacerMarker;
}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void ECGParam::setSweepSpeed(ECGSweepSpeed speed)
{
    systemConfig.setNumValue("PrimaryCfg|ECG|SweepSpeed", (int)speed);

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setSpeed(speed);
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        spo2Param.setSweepSpeed(speed);
    }

    windowManager.resetWave();
}

/**************************************************************************************************
 * 获取波形速度。
 *************************************************************************************************/
ECGSweepSpeed ECGParam::getSweepSpeed(void)
{
    int speed = ECG_SWEEP_SPEED_250;
    systemConfig.getNumValue("PrimaryCfg|ECG|SweepSpeed", speed);
    return (ECGSweepSpeed)speed;
}

/**************************************************************************************************
 * 设置增益。
 *************************************************************************************************/
void ECGParam::setGain(ECGGain gain, int waveID)
{
    setGain(gain, waveIDToLeadID((WaveformID)waveID));
}

/**************************************************************************************************
 * 设置增益。
 *************************************************************************************************/
void ECGParam::setGain(ECGGain gain, ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return;
    }

    if (_waveWidget[lead] == NULL)
    {
        return;
    }

    QString wavename = _waveWidget[lead]->name();
    systemConfig.setNumValue("PrimaryCfg|ECG|Gain|" + wavename, (int)gain);
    _waveWidget[lead]->setGain(gain);
}

/**************************************************************************************************
 * 设置12导界面下波形的增益。
 *************************************************************************************************/
void ECGParam::set12LGain(ECGGain gain, ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return;
    }

    if (_waveWidget[lead] == NULL)
    {
        return;
    }

    _waveWidget[lead]->set12LGain(gain);
    _waveWidget[lead]->setGain(gain);
}

/**************************************************************************************************
 * 设置12导界面下波形的增益。
 *************************************************************************************************/
ECGGain ECGParam::get12LGain(ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return ECG_GAIN_X10;
    }

    if (_waveWidget[lead] == NULL)
    {
        return ECG_GAIN_X10;
    }

    return _waveWidget[lead]->get12LGain();
}

/**************************************************************************************************
 * 获取增益。
 *************************************************************************************************/
ECGGain ECGParam::getGain(ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return ECG_GAIN_X10;
    }

    if (_waveWidget[lead] == NULL)
    {
        return ECG_GAIN_X10;
    }

    QString waveName = _waveWidget[lead]->name();

    int gain = ECG_GAIN_X10;
    systemConfig.getNumValue("PrimaryCfg|ECG|Gain|" + waveName, gain);
    return (ECGGain)gain;
}

/**************************************************************************************************
 * 设置自动增益。
 *************************************************************************************************/
void ECGParam::setAutoGain(ECGLead lead, int flag)
{
    if (lead > ECG_LEAD_V6)
    {
        return ;
    }

    if (_waveWidget[lead] == NULL)
    {
        return;
    }

    QString waveName = _waveWidget[lead]->name();

    systemConfig.setNumValue("PrimaryCfg|ECG|AutoGain|" + waveName, flag);
}

/**************************************************************************************************
 * 获取自动增益。
 *************************************************************************************************/
bool ECGParam::getAutoGain(ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return false;
    }

    if (_waveWidget[lead] == NULL)
    {
        return false;
    }

    QString waveName = _waveWidget[lead]->name();

    int gain = 0;
    systemConfig.getNumValue("PrimaryCfg|ECG|AutoGain|" + waveName, gain);
    return (bool)gain;
}

/**************************************************************************************************
 * 非监护模式下修改自动增益。
 *************************************************************************************************/
void ECGParam::changeAutoGain(void)
{
    // 获取支持的导联。
    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (getAutoGain((ECGLead)i))
        {
            if (NULL != _waveWidget[(ECGLead)i])
            {
                setAutoGain((ECGLead)i, false);
                _waveWidget[(ECGLead)i]->setAutoGain(false);
                _waveWidget[(ECGLead)i]->setGain(ecgParam.getGain((ECGLead)i));
            }
        }
    }
}

/**************************************************************************************************
 * 获取最大的增益值。
 *************************************************************************************************/
int ECGParam::getMaxGain(void)
{
    return ECG_GAIN_NR;
}

/**************************************************************************************************
 * 设置QRS音量。
 *************************************************************************************************/
void ECGParam::setQRSToneVolume(int vol)
{
    superRunConfig.setNumValue("ECG|QRSVolume", (int)vol);
    soundManager.setSoundVolume(SOUND_VOLUME_CHANNEL_HEART_BEAT, (SoundVolume)vol);
}

/**************************************************************************************************
 * 获取QRS音量。
 *************************************************************************************************/
int ECGParam::getQRSToneVolume(void)
{
    int vol = SOUND_VOL_3;
    superRunConfig.getNumValue("ECG|QRSVolume", vol);
    return vol;
}

/**************************************************************************************************
 * 设置/获取工频滤波。
 *************************************************************************************************/
void ECGParam::setNotchFilter(int filter)
{
    if (_curLeadMode != ECG_LEAD_MODE_12)
    {
        filter = ECG_NOTCH_50_AND_60HZ;
    }
    else
    {
        if (ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
        {
            superRunConfig.setNumValue("ECG12L|NotchFilter", filter);
        }
        else
        {
            filter = ECG_NOTCH_50_AND_60HZ;
        }
    }

    if (NULL != _provider)
    {
        _provider->setNotchFilter((ECGNotchFilter)filter);
    }
}

/**************************************************************************************************
 * 设置/获取工频滤波。
 *************************************************************************************************/
ECGNotchFilter ECGParam::getNotchFilter()
{
    int filter = ECG_NOTCH_50_AND_60HZ;

    if (ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
    {
        superRunConfig.getNumValue("ECG12L|NotchFilter", filter);
    }

    return (ECGNotchFilter)filter;
}

/***************************************************************************************************
 * get the notch filter of the calc lead :
 **************************************************************************************************/
ECGNotchFilter ECGParam::getCalcLeadNotchFilter()
{
    return ECG_NOTCH_50_AND_60HZ;
}

/**************************************************************************************************
 * 获取导联标签标准。
 *************************************************************************************************/
ECGLeadNameConvention ECGParam::getLeadConvention(void) const
{
    int leadConvention = 0;
    superConfig.getNumValue("ECG|ECGLeadConvention", leadConvention);
    if (leadConvention >= ECG_CONVENTION_NR)
    {
        leadConvention = 0;
    }

    return (ECGLeadNameConvention)leadConvention;
}

/**************************************************************************************************
 * 获取12导联显示格式。
 *************************************************************************************************/
Display12LeadFormat ECGParam::get12LDisplayFormat(void) const
{
    return _12LeadDispFormat;
}

/***************************************************************************************************
 * 对导联线类型做相应处理
 **************************************************************************************************/
void ECGParam::handleECGLeadCabelType(unsigned char cabelType)
{
    ECGLeadMode leadMode = getECGModeByECGCabelType(cabelType);

    if ((ECG_LEAD_MODE_NR <= leadMode) || (ECG_LEAD_MODE_3 > leadMode))
    {
        //如果是新规划，并且是开机第一次判断，则如果导联现不能识别就设置为3导，否则保持上一次不变
        if (_isPowerOnNewSession && (POWER_ON_SESSION_NEW == timeManager.getPowerOnSession()))
        {
            leadMode = ECG_LEAD_MODE_3;
        }
        else
        {
            return ;
        }
    }

    if (ECG_LEAD_MODE_12 == leadMode)
    {
        int ecg12LEnable = 0;
        machineConfig.getNumValue("ECG12LEADEnable", ecg12LEnable);
        if (0 == ecg12LEnable)
        {
            leadMode = ECG_LEAD_MODE_5;
        }
    }

    _isPowerOnNewSession = false;
    setLeadMode(leadMode);
    QTimer::singleShot(0, &ecgMenu, SLOT(_loadOptionsInSlot()));

    return ;
}

/***************************************************************************************************
 * 通过导联线类型获取导联模式
 **************************************************************************************************/
ECGLeadMode ECGParam::getECGModeByECGCabelType(unsigned char cabelType)
{
    ECGLeadMode leadMode = ECG_LEAD_MODE_3;

    switch (cabelType) {
    case (0x00):
        leadMode = ECG_LEAD_MODE_NR;
        break;

    case (0x01):
        leadMode = ECG_LEAD_MODE_3;
        break;

    case (0x02):
        leadMode = ECG_LEAD_MODE_5;
        break;

    case (0x03):
        leadMode = ECG_LEAD_MODE_12;
        break;

    default:
        leadMode = ECG_LEAD_MODE_NR;
        break;
    }

    return leadMode;
}

/***************************************************************************************************
 * is ever lead on
 **************************************************************************************************/
bool ECGParam::getEverLeadOn(ECGLead lead)
{
    if (lead <= ECG_LEAD_AVF)
    {
        for (int i = 0; i <= lead; ++i)
        {
            if (_isEverLeadOn[lead])
            {
                return true;
            }
        }

        return false;
    }

    return _isEverLeadOn[lead];
}

/***************************************************************************************************
 * set VF signal
 **************************************************************************************************/
void ECGParam::setCheckPatient(bool flag)
{
    if (_isCheckPatient == flag)
    {
        return;
    }

    _isCheckPatient = flag;
}

/***************************************************************************************************
 * presentRhythm : presenting rhythm
 **************************************************************************************************/
void ECGParam::presentRhythm()
{
    int rhythm = 0;
    systemConfig.getNumValue("PrimaryCfg|ECG|PresentRhythm", rhythm);
    //check first lead on during the incident
    if(!rhythm)
    {
        rhythm = 1;
        systemConfig.setNumValue("PrimaryCfg|ECG|PresentRhythm", rhythm);
        summaryStorageManager.addPrensentingRhythm(timeManager.getCurTime());
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGParam::ECGParam() : Param(PARAM_ECG)
{
    // 初始化成员。
    _provider = NULL;
    _pvcsTrendWidget = NULL;
    _ecgSTTrendWidget = NULL;
    _waveDataInvalid = true;
    _isDisableDiaSoftKey = false;
    _isCheckPatient = false;
    _hrValue = InvData();
    memset(_leadOff, 2, sizeof(_leadOff));

    _monitorSoftkey = (MonitorSoftkeyAction*)softkeyManager.getAction(SOFTKEY_ACTION_STANDARD);

//    // 关闭/使能12导的快捷软按键。
//    if (_monitorSoftkey != NULL)
//    {
//        int ecg12LEnable = 0;
//        machineConfig.getNumValue("ECG12LEADEnable", ecg12LEnable);
//        if (ecg12LEnable)
//        {
//            _monitorSoftkey->enable12Lead(true);
//        }
//        else
//        {
//            _monitorSoftkey->enable12Lead(false);
//        }
//    }

    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        _waveWidget[i] = NULL;
    }

    int lead = ECG_LEAD_II;
    superRunConfig.getNumValue("ECG|DefaultECGLeadInMonitorMode", lead);
    _calcLead = (ECGLead)lead;

    int mode = ECG_LEAD_MODE_5;
    systemConfig.getNumValue("PrimaryCfg|ECG|LeadMode", mode);
    _curLeadMode = (ECGLeadMode)mode;

    mode = ECG_DISPLAY_NORMAL;
    systemConfig.getNumValue("PrimaryCfg|ECG|DisplayMode", mode);
    _displayMode = (ECGDisplayMode)mode;

    mode = ECG_PACE_OFF;
    systemConfig.getNumValue("PrimaryCfg|ECG|ECG12LeadPacerMaker", mode);
    _12LeadPacerMarker = (ECGPaceMode) mode;

    mode = ECG_BANDWIDTH_067_20HZ;
    superConfig.getNumValue("ECG|ChestLeadsECGBandwidth", mode);
    _chestFreqBand = (ECGBandwidth) mode;

    mode = ECG_FILTERMODE_MONITOR;
    superConfig.getNumValue("PrimaryCfg|ECG|FilterMode", mode);
    _filterMode = (ECGFilterMode) mode;

    mode = 0;
    superConfig.getNumValue("ECG12L|ECG12LeadBandwidth", mode);
    mode += ECG_BANDWIDTH_0525_40HZ;
    _12LeadFreqBand = (ECGBandwidth) mode;

    mode = DISPLAY_12LEAD_STAND;
    superConfig.getNumValue("ECG12L|DisplayFormat", mode);
    _12LeadDispFormat = (Display12LeadFormat)mode;

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        QString everLeadOnStr = "PrimaryCfg|ECG|EverLeadOn|";
        everLeadOnStr += ECGSymbol::convert((ECGLead)i, ECG_CONVENTION_AAMI);
        systemConfig.getNumValue(everLeadOnStr, _isEverLeadOn[i]);
    }

    _lastDiagModeTimestamp = 0;

//    _lastCabelType = 0x00;
    _isPowerOnNewSession = true;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGParam::~ECGParam()
{
    _timer.stop();
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void ECGParam::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    if (NULL != _provider)
    {
        _provider->sendCmdData(cmdId,data,len);
    }
}