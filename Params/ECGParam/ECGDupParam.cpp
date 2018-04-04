#include "ECGDupParam.h"
#include "BaseDefine.h"
#include "IConfig.h"
#include "ECGTrendWidget.h"
#include "Debug.h"
#include "ECGParam.h"
#include "SoundManager.h"

ECGDupParam *ECGDupParam::_selfObj = NULL;

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void ECGDupParam::initParam(void)
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void ECGDupParam::handDemoWaveform(WaveformID /*id*/, short /*data*/)
{
}

/**************************************************************************************************
 * 产生DEMO数据。
 *************************************************************************************************/
void ECGDupParam::handDemoTrendData(void)
{

}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
void ECGDupParam::getAvailableWaveforms(QStringList &/*waveforms*/,
                                         QStringList &/*waveformShowName*/, int /*flag*/)
{

}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short ECGDupParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
        case SUB_PARAM_HR_PR:
            return getHR();

        default:
            return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void ECGDupParam::showSubParamValue()
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showValue();
    }
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void ECGDupParam::setProvider(ECGProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;
}


/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType ECGDupParam::getCurrentUnit(SubParamID /*id*/)
{
    return UNIT_BPM;
}

/**************************************************************************************************
 * 设置趋势窗体。
 *************************************************************************************************/
void ECGDupParam::setTrendWidget(ECGTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _trendWidget = trendWidget;
}

/**************************************************************************************************
 * 更新PR的值。
 *************************************************************************************************/
void ECGDupParam::updatePR(short pr)
{
    _prValue = pr;
    soundManager.setPr(_prValue);

    if (_trendWidget == NULL)
    {
        return;
    }

    // HR is valid。
    if (_hrValue != InvData())
    {
        return;
    }

    // 当HR为无效值时才使用PR。
    if (_prValue != InvData())
    {
        _hrBeatFlag = false;
        _trendWidget->setHRValue(_prValue, false);
    }
    else //HR & PR all invalid
    {
        _hrBeatFlag = false;
        _trendWidget->setHRValue(_prValue, true);
    }
}

/**************************************************************************************************
 * 更新VFVT数值。
 *************************************************************************************************/
void ECGDupParam::updateVFVT(bool /*onoff*/)
{
    return;
}

/**************************************************************************************************
 * 更新HR心跳图标。
 *************************************************************************************************/
void ECGDupParam::updateHRBeatIcon()
{
    if (_hrBeatFlag)
    {
        if (NULL != _trendWidget && _hrValue != InvData())
        {
            _trendWidget->blinkBeatPixmap();
        }
    }
}

/**************************************************************************************************
 * 更新PR心跳图标。
 *************************************************************************************************/
void ECGDupParam::updatePRBeatIcon()
{
    if (!_hrBeatFlag)
    {
        if (NULL != _trendWidget && _prValue != InvData())
        {
            _trendWidget->blinkBeatPixmap();
        }
    }
}

/**************************************************************************************************
 * 更新HR的值。
 *************************************************************************************************/
void ECGDupParam::updateHR(short hr)
{
    _hrValue = hr;
    soundManager.setHr(_hrValue);

    if (_trendWidget == NULL)
    {
        return;
    }

    // HR不为无效时即显示。
    if (_hrValue != InvData())
    {
        _hrBeatFlag = true;
        _trendWidget->setHRValue(_hrValue, true);
    }
    else if (_prValue != InvData())
    {
        _hrBeatFlag = false;
        _trendWidget->setHRValue(_prValue, false);
    }
    else // HR和PR都为无效时。
    {
        _hrBeatFlag = true;
        _trendWidget->setHRValue(_hrValue, true);
    }
}

/**************************************************************************************************
 * 获取HR的值。
 *************************************************************************************************/
short ECGDupParam::getHR(void) const
{
    if (InvData() != _hrValue)
    {
        return _hrValue;
    }

    if (InvData() != _prValue)
    {
        return _prValue;
    }

    return InvData();
}

/**************************************************************************************************
 * 是否为HR有效。
 *************************************************************************************************/
bool ECGDupParam::isHRValid(void)
{
    if (_hrValue != InvData())
    {
        return true;
    }

    if (_prValue == InvData())
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void ECGDupParam::isAlarm(bool isAlarm, bool isLimit)
{
    _isAlarm |= isAlarm;
    if (isLimit)
    {
        return;
    }

    if (NULL != _trendWidget)
    {
        _trendWidget->isAlarm(_isAlarm);
        _isAlarm = false;
    }
}

/***************************************************************************************************
 * get the hr source
 **************************************************************************************************/
ECGDupParam::HrSourceType ECGDupParam::getHrSource() const
{
    if (_hrValue == InvData() && _prValue != InvData())
    {
        return HR_SOURCE_SPO2;
    }
    else
    {
        return HR_SOURCE_ECG;
    }
}


/**************************************************************************************************
 * 设置计算导联字串。
 *************************************************************************************************/
void ECGDupParam::setECGTrendWidgetCalcName(ECGLead calLead)
{
    _trendWidget->setTrendWidgetCalcName(calLead);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGDupParam::ECGDupParam() : Param(PARAM_DUP_ECG)
{
    // 初始化成员。
    _trendWidget = NULL;
    _provider = NULL;
    _hrValue = InvData();
    _prValue = InvData();
    _hrBeatFlag = true;
    _isAlarm = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGDupParam::~ECGDupParam()
{

}