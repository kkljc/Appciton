/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include <QPalette>
#include "Param.h"
#include "Framework/Utility/Unit.h"
#include "AlarmDefine.h"
#include "IBPSymbol.h"
#include "IBPDefine.h"
#include "IBPProviderIFace.h"
#include "PromptInfoBarWidget.h"

class IBPWaveWidget;
class IBPTrendWidget;
class IBPMenu;
class IBPParam: public Param
{
    Q_OBJECT
public:
    static IBPParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new IBPParam();
        }
        return *_selfObj;
    }
    ~IBPParam();

    QList<IBPScaleInfo> ibpScaleList;                      // all scale for ibp

public:
    // 处理demo数据
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);
    /* reimplement */
    virtual void exitDemo();

    // 显示
    virtual void showSubParamValue();

    // 是否超限报警
    void noticeLimitAlarm(int id, bool isAlarm, IPBChannel chn);

    // 获取可得的波形控件集。
    virtual void getAvailableWaveforms(QStringList *waveforms,
                                       QStringList *waveformShowName, int);

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // check whether the sub param is avaliabe
    virtual bool isSubParamAvaliable(SubParamID id);

    // 设置数据提供对象。
    void setProvider(IBPProviderIFace *provider);

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // 是否连接了真实的Provider，是指Provider有数据产生。
    bool isConnected(void);

    // 设置测量实时数据。
    void setRealTimeData(unsigned short sys, unsigned short dia, unsigned short map,
                         unsigned short pr, IPBChannel chn);

    // 设置波形值。
    void addWaveformData(short wave, bool invalid, IPBChannel chn);

    // 设置界面对象
    void setIBPTrendWidget(IBPTrendWidget *trendWidget, IPBChannel chn);
    void setWaveWidget(IBPWaveWidget *waveWidget, IPBChannel chn);

    /*reimplement*/
    QList<SubParamID> getShortTrendList(IPBChannel chn);

    /**
     * @brief getIBPScale get the IBP scale info
     * @param name
     * @return
     */
    IBPScaleInfo getIBPScale(IBPLabel name);

    // 设置/获取波形标尺限。
    void setRulerLimit(IBPRulerLimit ruler, IPBChannel chn);
    void setRulerLimit(int low, int high, IPBChannel chn);
    IBPRulerLimit getRulerLimit(IPBChannel chn);
    IBPRulerLimit getRulerLimit(IBPLabel name);

    // 设置/获取标尺信息
    void setScaleInfo(const IBPScaleInfo &info, const IBPLabel &name);
    IBPScaleInfo &getScaleInfo(IPBChannel chn);

    // 清空校准相关报警
    void clearCalibAlarm(void);

    /**
     * @brief hasIBPZeroReply check whether received ibp zero reply
     * @param chn the channel
     * @return true when got reply
     */
    bool hasIBPZeroReply(IPBChannel chn);

    /**
     * @brief getMoudleType get the connect module type
     * @return the module type
     */
    IBPModuleType getMoudleType() const;

public:
    // 校零。
    void zeroCalibration(IPBChannel chn);

    /**
     * @brief hasBeenZero check whether the channel has been zero or not
     * @param chn the ibp channel
     * @return true if the channel is already zeroed
     */
    bool hasBeenZero(IPBChannel chn);

    // 校准
    void setCalibration(IPBChannel chn, unsigned short value);

    // 校零校准信息
    void setCalibrationInfo(IBPCalibration calib, IPBChannel chn, int calibinfo);

    // 导联状态
    void leadStatus(bool staIBP1, bool staIBP2);

    /**
     * @brief isIBPLeadOff  获取ibp导联脱落状态
     * @param chn  ibp通道
     * @return  导联脱落状态
     */
    bool isIBPLeadOff(IPBChannel chn);

    // 设置/获取波形速度。
    void setSweepSpeed(IBPSweepSpeed speed);
    IBPSweepSpeed getSweepSpeed(void);

    // 设置标名。
    void setEntitle(IBPLabel entitle, IPBChannel chn);

    // get pressure name
    IBPLabel getEntitle(IPBChannel signal) const;
    IBPLabel getEntitle(IBPLimitAlarmType alarmType) const;

    // 设置滤波
    void setFilter(IBPFilterMode filter);
    IBPFilterMode getFilter(void);

    // 设置灵敏度
    void setSensitivity(IBPSensitivity sensitivity);
    IBPSensitivity getSensitivity(void);

    // 设置获取IBP计算结果值
    IBPParamInfo getParamData(IPBChannel chn);
    void setParamData(IPBChannel chn, unsigned short sys, unsigned short dia,
                      unsigned short mean, unsigned short pr);

    /**
     * @brief getSubParamID 获取ibp两通道标名对应的参数ID。
     * @param inputID
     * @return
     */
    SubParamID getSubParamID(IPBChannel inputID);

    // 参数名获取标名
    IBPLabel getPressureName(SubParamID id);
    IBPLabel getPressureName(WaveformID id);
    SubParamID getSubParamID(IBPLabel name);
    WaveformID getWaveformID(IBPLabel name);

    // 刷新参数上下限
    virtual void updateSubParamLimit(SubParamID id);
private slots:
    void onPaletteChanged(ParamID id);

private:
    IBPParam();
    static IBPParam *_selfObj;
    void _setWaveformSpeed(IBPSweepSpeed speed);

    IBPProviderIFace *_provider;

    IBPWaveWidget *_waveWidgetIBP1;
    IBPWaveWidget *_waveWidgetIBP2;

    IBPTrendWidget *_trendWidgetIBP1;
    IBPTrendWidget *_trendWidgetIBP2;

    IBPParamInfo _ibp1;
    IBPParamInfo _ibp2;

    IBPScaleInfo _scale1;
    IBPScaleInfo _scale2;

    bool _staIBP1;                        // 导联状态
    bool _staIBP2;
    bool _connectedProvider;

    bool _ibp1ZeroReply;                // 校零回复
    bool _ibp2ZeroReply;
    bool _ibp1HasBeenZero;              /* check whether the channel has been zero */
    bool _ibp2HasBeenZero;              /* check whether the channel has been zero */
};
#define ibpParam (IBPParam::construction())
