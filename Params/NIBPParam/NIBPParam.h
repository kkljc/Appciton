#pragma once
#include "Param.h"
#include "NIBPSymbol.h"
#include "NIBPState.h"
#include "UnitManager.h"
#include "PatientManager.h"
#include "TimeManager.h"
#include "NIBPEventTrigger.h"
#include "NIBPCountdownTime.h"
#include <QMap>

class NIBPProviderIFace;
class NIBPTrendWidget;
class NIBPDataTrendWidget;
class NIBPParam: public Param
{
    Q_OBJECT
#ifdef CONFIG_UNIT_TEST
    friend class TestNIBPParam;
#endif
public:
    static NIBPParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new NIBPParam();
        }
        return *_selfObj;
    }
    static NIBPParam *_selfObj;
    ~NIBPParam();

public:
    // 解析数据包。
    void unPacket(unsigned char *packet, int len);

    // 状态机运行。
    void machineRun(void);

    // 切换虚拟机状态。
    void switchState(NIBPStateType state);
    NIBPStateType currentState();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoTrendData(void);

    // 获取子参数值
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(NIBPProviderIFace *provider);

    // 模块复位
    void reset();

    // NIBP错误,模块禁用
    void errorDisable(void);
    bool isErrorDisable(void) {return _isNIBPDisable;}

    // 是否连接
    bool isConnected();

    // check whether current is in measure state
    bool isMeasuring() const;

    // 通信超时
    void connectedTimeout(void);

    // 设置界面对象。
    void setNIBPTrendWidget(NIBPTrendWidget *trendWidget);
    void setNIBPDataTrendWidget(NIBPDataTrendWidget *trendWidget);

    // 设置测量结果。
    void setResult(short sys, short dia, short map, short pr, NIBPOneShotType err);

    // 设置最后测量时间。
    void setLastTime(void);

    // 清除测量结果。
    void clearResult(void);

    // 清除显示值。
    void invResultData(void);    

    // 袖带压力。
    void setCuffPressure(int pressure);

    // 更新倒计时信息，秒为单位。
    void setCountdown(int t);

    // NIBP状态信息显示。
    void setText(QString text);
    void setText(void);

    // NIBP 模式显示。
    void setModelText(QString text);

    //上次测量时间的显示
    void setShowMeasureCount(void);

    // 获得NIBP的测量数据。
    short getSYS(void);
    short getDIA(void);
    short getMAP(void);
    short getPR(void);
    NIBPMeasureResult getMeasureResult(void);
    void setMeasureResult(NIBPMeasureResult flag);

    //测量时间切换标志
    bool isSwitchTime(void);
    void setSwitchFlagTime(bool flag);

    //NIBP按钮标志
    bool isSwitchType(void);
    void setSwitchFlagType(bool flag);

    //额外触发
    void setAdditionalMeasure(bool flag);
    bool isAdditionalMeasure(void);

    //STAT模式在5分钟时间未到的情况下被手动关闭的标志
    void setSTATClose(bool flag);
    bool isSTATClose(void);

    //自动模式中的手动触发测量
    void setAutoMeasure(bool flag);
    bool isAutoMeasure(void);

    //在安全模式时,打开STAT模式标志,但模式未运行,在安全模式下临时打开
    void setSTATOpenTemp(bool flag);
    bool isSTATOpenTemp(void);

    //以STAT启动测量的第一次（测量中转STAT的不算第一次）
    void setSTATFirst(bool flag);
    bool isSTATFirst(void);

    //STAT测量模式
    void setSTATMeasure(bool flag);
    bool isSTATMeasure(void);

    //超限报警通知
    void noticeLimitAlarm(int id, bool flag);

    //create a nibp snapshot
    void setSnapshotFlag(bool flag);
    bool isSnapshotFlag(void);
    void createSnapshot(NIBPOneShotType err);

public:
    // 根据病人类型获取对应的初始压力值。
    int getInitPressure(PatientType type);
    void setInitPressure(int index);

    // 设置/获取测量模式。
    void setMeasurMode(NIBPMode mode);
    NIBPMode getMeasurMode(void);
    NIBPMode getSuperMeasurMode(void);

    // 设置/获取自动测量时的时间间隔。
    void setAutoInterval(NIBPAutoInterval interv);
    NIBPAutoInterval getAutoInterval(void);
    int getAutoIntervalTime(void);

    // 设置/获取智能充气模式。
    void setIntelligentInflate(NIBPIntelligentInflate mode);
    NIBPIntelligentInflate getIntelligentInflate(void);

    // 获取单位。
    UnitType getUnit(void);

    // 停止测量。
    void stopMeasure(void);

    //按钮释放
    void keyReleased(void);

    //按钮按下
    void keyPressed(void);

    // 短按NIBP触发 测量起停控制，起停反操作：如果当前处于测量则停止，当前为停止则开始测量。
    void toggleMeasureShort(void);

    //长按NIBP触发
    void toggleMeasureLong(void);

private slots:
    void _patientTypeChangeSlot(PatientType type);
    void _btnTimeOut();

private:
    NIBPParam();

    NIBPProviderIFace *_provider;
    NIBPTrendWidget *_trendWidget;
    NIBPDataTrendWidget *_nibpDataTrendWidget;

    // 设置测量结果的数据。
    void transferResults(short sys, short dia, short map, unsigned time);

    short _sysValue;
    short _diaValue;
    short _mapVaule;
    short _prVaule;
    unsigned _lastTime;
    NIBPMeasureResult _measureResult;
    bool _SwitchFlagTime;                   // 切换时间标志
    bool _SwitchFlagType;                   // 切换病人类型标志
    bool _additionalMeasureFlag;            // 额外充气标志
    bool _autoMeasureFlag;                  // 自动模式中的手动测量标志
    bool _statModelFlag;                    // STAT模式打开关闭标志
    bool _statCloseFlag;                    // STAT模式在5分钟时间未到的情况下被手动关闭的标志
    bool _statFirst;                        // 第一次启动STAT
    bool _toggleMeasureLongFlag;            // 长按触发生效标志
    bool _statOpenTemp;                     // 在安全模式时,打开STAT模式标志,但模式未运行,在安全模式下临时打开
    bool _isCreateSnapshotFlag;             // 创建快拍标记；
    bool _isNIBPDisable;                    // 模块禁用；
    bool _isManualMeasure;                  // 手动测量标志
    QString _text;

private:
    // 状态机相关。
    typedef QMap<NIBPStateType, NIBPState*> StateMap;
    StateMap _states;
    NIBPState *_currentState ;

    QTimer *_btnTimer;


};
#define nibpParam (NIBPParam::construction())