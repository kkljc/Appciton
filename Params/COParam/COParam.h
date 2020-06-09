/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include "Param.h"
#include "COProviderIFace.h"
#include <QScopedPointer>

template <typename T> class QVector;
/**
 * @brief The COMeasureData struct record the result of single measurement
 */
struct COMeasureData
{
    COMeasureData() : timestamp(0), co(InvData()), ci(InvData()) {}
    unsigned timestamp; /* measure timestamp */
    short co;   /* measure co value */
    short ci;   /* measure ci value */
    QVector<short> measureWave;
};

class COTrendWidget;
class COMenu;
class COParamPrivate;
class COParam : public Param
{
    Q_OBJECT
public:
    static COParam &getInstance();
    ~COParam();

public:
    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);
    /* reimplement */
    virtual void exitDemo();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 获取子参数值。
    virtual int16_t getSubParamValue(SubParamID id);

    // 设置数据提供对象。
    void setProvider(COProviderIFace *provider);

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // 是否连接了真实的Provider，是指Provider有数据产生。
    bool isConnected(void);

    // 设置界面对象
    void setCOTrendWidget(COTrendWidget *trendWidget);

public:
    /**
     * @brief setCatheterCoeff set the Swan-Ganz catheter coefficient
     * @param coef the coefficient
     * @note
     * The coeffficient should be multiplied by 1000, in range of (0~999)
     */
    void setCatheterCoeff(unsigned short coef);

    /**
     * @brief getCatheterCoeff get the Swan-Ganz catheter coefficient
     * @return the coefficient
     * @note
     * The coeffficient should be multiplied by 1000, in range of (0~999)
     */
    unsigned short getCatheterCoeff(void);

    /**
     * @brief setTiSource set the ti source
     * @param source current source
     * @param temp only use when the source is manual
     */
    void setTiSource(COTiSource source, unsigned short temp = 0);

    /**
     * @brief getTiSource get current ti source
     * @return The ti source
     */
    COTiSource getTiSource() const;

    /**
     * @brief setInjectionVolume set the injection volume
     * @param volume volume of injection, in unit of ml
     */
    void setInjectionVolume(unsigned char volume);

    /**
     * @brief getInjectionVolume get the current injection volume
     * @return the injection volume
     */
    unsigned char getInjectionVolume(void) const;

    /**
     * @brief measureCtrl measure contorl
     * @param ctrl
     */
    void measureCtrl(COMeasureCtrl ctrl);
    COMeasureCtrl getMeasureCtrl(void);

    /**
     * @brief startMeasure start co measure
     */
    void startMeasure();

    /**
     * @brief isMeasuring check whether co is in measuring state
     * @return true when in measuring state, otherwise, false
     */
    bool isMeasuring() const;

    /**
     * @brief stopMeasure stop co measure
     */
    void stopMeasure();

    /**
     * @brief setMeasureResult set the measure result of single measurement
     * @param co the cardiac output
     * @param ci the cardiac index
     */
    void setMeasureResult(short co, short ci);

    /**
     * @brief setTb set the blood temperature
     * @param tb the blood temperature
     * @note
     * The value should be unit of 0.1 celsius degree
     */
    void setTb(short tb);

    /**
     * @brief getTb get current blood temperature
     * @return the blood temperature
     */
    short getTb(void) const;

    /**
     * @brief setTi set the injection temperature
     * @param ti the injection temperature
     * @note
     * Use to update the TI by module when the TI source is auto
     */
    void setTi(short ti);

    /**
     * @brief getTi get the current ti value
     * @return the current ti
     */
    unsigned short getTi(void) const;

    /**
     * @brief getAvgCo get the average Cardiac output
     * @return  current cardiac output or InvData()
     * @note
     * Valid return value is already scaled by 10
     */
    short getAvgCo(void) const;

    /**
     * @brief getAvgCi get the average Cardiac index
     * @return  current cardiac index of InvData()
     * @note
     * Valid return value is already scaled by 10
     */
    short getAvgCi(void) const;

    /**
     * @brief addMeasureWaveData add the measurement wave data
     * @param data the wave data
     */
    void addMeasureWaveData(short data);

    /**
     * @brief setOneshotAlarm set the oneshot alarm status
     * @param t the oneshot alarm type
     * @param f the oneshot alarm status
     */
    void setOneshotAlarm(COOneShotType t, bool f);

private:
    COParam();

    QScopedPointer<COParamPrivate> pimpl;
};

#define coParam (COParam::getInstance())
