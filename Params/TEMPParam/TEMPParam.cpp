#include "TEMPParam.h"
#include "TEMPAlarm.h"
#include "TEMPTrendWidget.h"
#include "TEMPProviderIFace.h"
#include "IConfig.h"
#include "WaveformCache.h"
#include "FactoryTempMenu.h"
#include "SystemManager.h"

TEMPParam *TEMPParam::_selfObj = NULL;

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void TEMPParam::initParam(void)
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void TEMPParam::handDemoWaveform(WaveformID /*id*/, short /*data*/)
{
    return;
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void TEMPParam::handDemoTrendData(void)
{
    _t1Value = qrand() % 10 + 90;
    _t2Value = qrand() % 55 + 45;
    _tdValue = abs(_t1Value - _t2Value);
    if (NULL != _trendWidget)
    {
        _trendWidget->setTEMPValue(_t1Value, _t2Value, _tdValue);
    }
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short TEMPParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
        case SUB_PARAM_T1:
            return _t1Value;

        case SUB_PARAM_T2:
            return _t2Value;

        case SUB_PARAM_TD:
            return _tdValue;

        default:
            return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void TEMPParam::showSubParamValue()
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showValue();
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType TEMPParam::getCurrentUnit(SubParamID /*id*/)
{
    return getUnit();
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void TEMPParam::setProvider(TEMPProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;

    _provider->sendTEMPSelfTest();
    _provider->sendProbeState();
}

/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void TEMPParam::reset()
{
    if (NULL == _provider)
    {
        return;
    }

    _provider->sendTEMPSelfTest();
    _provider->sendProbeState();
}

/**************************************************************************************************
 * 通信中断。
 *************************************************************************************************/
bool TEMPParam::getDisconnected()
{
    return _provider->isDisconnected();
}

/**************************************************************************************************
 * 模块错误,模块被禁用。
 *************************************************************************************************/
void TEMPParam::setErrorDisable()
{
    _isTEMPDisable = true;
}

/**************************************************************************************************
 * 设置服务模式升级数据提供对象。
 *************************************************************************************************/
void TEMPParam::setServiceProvider(TEMPProviderIFace *provider)
{
    _provider = provider;
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void TEMPParam::setTrendWidget(TEMPTrendWidget *trendWidget)
{
    _trendWidget = trendWidget;
}

/**************************************************************************************************
 * 设置TEMP的值。
 *************************************************************************************************/
void TEMPParam::setTEMP(short t1, short t2, short td)
{
    if (_isTEMPDisable)
    {
        _t1Value = InvData();
        _t1Value = InvData();
        _tdValue = InvData();
    }
    else
    {
        _t1Value = t1;
        _t2Value = t2;
        _tdValue = td;
    }
    if (NULL != _trendWidget)
    {
        _trendWidget->setTEMPValue(t1, t2, td);
    }
    factoryTempMenu.setTEMPValue(t1, t2);
}

/**************************************************************************************************
 * 获取TEMP的值。
 *************************************************************************************************/
void TEMPParam::getTEMP(short &t1, short &t2, short &td)
{
    t1 = _t1Value;
    t2 = _t2Value;
    td = _tdValue;
}

/**************************************************************************************************
 * 设置报警。
 *************************************************************************************************/
void TEMPParam::setOneShotAlarm(TEMPOneShotType t, bool f)
{
    if (f)
    {
        if (t == TEMP_ONESHOT_ALARM_COMMUNICATION_STOP || t == TEMP_ONESHOT_ALARM_MODULE_DISABLE)
        {
            tempOneShotAlarm.clear();
        }
    }
    tempOneShotAlarm.setOneShotAlarm(TEMP_ONESHOT_ALARM_MODULE_DISABLE,_isTEMPDisable);
    if (t >= TEMP_OVER_RANGR_1)
    {
        tempOneShotAlarm.setOneShotAlarm(t, f);
    }
}

/**************************************************************************************************
 * 获取趋势界面。
 *************************************************************************************************/
void TEMPParam::getTrendWindow(QString &trendWin)
{
    if (NULL != _trendWidget)
    {
        trendWin = _trendWidget->name();
    }
}

/**************************************************************************************************
 * 超限报警通知。
 *************************************************************************************************/
void TEMPParam::noticeLimitAlarm(int id, bool flag)
{
    if (NULL != _trendWidget)
    {
        _trendWidget->isAlarm(id, flag);
    }
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void TEMPParam::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    if (NULL != _provider)
    {
        _provider->sendCmdData(cmdId,data,len);
    }
}

void TEMPParam::sendCalibrateData(int channel, int value)
{
    _calibrateChannel = channel;
    _calibrateValue = value;
    _provider->sendCalibrateData(channel,value);
}

void TEMPParam::getCalibrateData(unsigned char *packet)
{
    if (_calibrateChannel != packet[1] || _calibrateValue != packet[2])
    {
        return;
    }
    factoryTempMenu.getResult(packet[1],packet[2],packet[3]);
}

/**************************************************************************************************
 * 设置单位。
 *************************************************************************************************/
void TEMPParam::setUnit(UnitType u)
{
    superConfig.setNumValue("Display|TEMPUnit", (int)u);

    if (NULL != _trendWidget)
    {
        _trendWidget->setUNit(u);
        _trendWidget->setTEMPValue(_t1Value, _t2Value, _tdValue);
    }
}

/**************************************************************************************************
 * 获取单位。
 *************************************************************************************************/
UnitType TEMPParam::getUnit(void)
{
    int u = UNIT_TDC;
    superConfig.getNumValue("Local|TEMPUnit", u);

    return (UnitType)u;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TEMPParam::TEMPParam() : Param(PARAM_TEMP)
{
    _provider = NULL;
    _trendWidget = NULL;

    _t1Value = InvData();
    _t2Value = InvData();
    _tdValue = InvData();

    _isTEMPDisable = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TEMPParam::~TEMPParam()
{

}