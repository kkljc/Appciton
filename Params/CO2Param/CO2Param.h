#pragma once
#include <QPalette>
#include "Param.h"
#include "CO2Symbol.h"
#include "UnitManager.h"
#include "AlarmDefine.h"

class CO2TrendWidget;
class CO2WaveWidget;
class CO2ProviderIFace;
class CO2Param: public Param
{
    Q_OBJECT

public:
    static CO2Param &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new CO2Param();
        }
        return *_selfObj;
    }
    static CO2Param *_selfObj;
    ~CO2Param();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取可得的波形控件集。
    virtual void getAvailableWaveforms(QStringList &waveforms,
            QStringList &waveformShowName, int flag);
    virtual void getTrendWindow(QString &trendWin);
    virtual void getWaveWindow(QString &waveWin);

    // 获取子参数值
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(CO2ProviderIFace *provider);

    // 模块复位
    void reset();

    // 设置界面对象。
    void setTrendWidget(CO2TrendWidget *trendWidget);
    void setWaveWidget(CO2WaveWidget *waveWidget);

    // 设置获取EtCO2。
    short getEtCO2(void);
    void setEtCO2(short);

    // 设置获取FiCO2。
    short getFiCO2(void);
    void setFiCO2(short);

    // 获取BR。
    void setBR(short);

    // 设置获取实时大气压值。
    short getBaro(void);
    void setBaro(short);

    // 处理模块的状态与主机状态同步。
    void verifyApneanTime(ApneaAlarmTime time);
    void verifyWorkMode(CO2WorkMode mode);

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // 是否连接了真实的Provider，是指Provider有数据产生。
    bool isConnected(void);

    // 设置波形值。
    void addWaveformData(short wave, bool invalid);

    // 设置OneShot报警。
    void setOneShotAlarm(CO2OneShotType t, bool status);

    // 超限报警通知
    void noticeLimitAlarm(int id, bool flag);

public:
    // 校零。
    void zeroCalibration(void);

    // 设置/获取窒息时间。
    void setApneaTime(ApneaAlarmTime t);
    ApneaAlarmTime getApneaTime(void);

    // 获取参数开关开关状态
    bool getAwRRSwitch(void);

    // 设置/获取波形速度。
    void setSweepSpeed(CO2SweepSpeed speed);
    CO2SweepSpeed getSweepSpeed(void);

    // 获取波形模式。
    CO2SweepMode getSweepMode(void);

    // 设置/获取气体补偿。
    void setCompensation(CO2Compensation which, int v);
    int getCompensation(CO2Compensation which);

    // 设置/获取波形放大标尺。
    void setDisplayZoom(CO2DisplayZoom zoom);
    CO2DisplayZoom getDisplayZoom(void);

    // 设置/获取FiCO2显示。
    void setFiCO2Display(CO2FICO2Display disp);
    CO2FICO2Display getFICO2Display(void);

    // 获取单位。
    UnitType getUnit(void);

    // 设置/获取CO2开关
    void setCO2Switch(int onoff);
    bool getCO2Switch();

    //获取CO2
    short getEtCO2MaxValue();
    short getEtCO2MinValue();

private:
    CO2Param();
    void _setWaveformSpeed(CO2SweepSpeed speed);
    void _setWaveformZoom(CO2DisplayZoom zoom);

    void _getCO2RESPWins(QString &co2Trend, QString &co2Wave, QString &respTrend, QString &respWave);

    CO2ProviderIFace *_provider;
    CO2TrendWidget *_trendWidget;
    CO2WaveWidget *_waveWidget;

    short _etco2Value;
    short _fico2Value;
    short _etco2MaxVal;
    short _etco2MinVal;
    short _brVaule;
    short _baro;
    bool _connectedProvider;
    bool _co2Switch;
    UnitType _curUnit;
};
#define co2Param (CO2Param::construction())