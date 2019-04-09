/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/11/13
 **/

#include "PowerManager.h"
#include "BatteryAlarm.h"
#include "MessageBox.h"
#include "WindowManager.h"
#include <QTimer>
#include "LanguageManager.h"
#define TWO_MINUTE 1000 * 120
#define POWER_LIST_MAX_COUNT 3


PowerManger * PowerManger::_selfObj = NULL;
class PowerMangerPrivate
{
public:
    explicit PowerMangerPrivate(PowerManger * const q_ptr)
        : q_ptr(q_ptr), lowBattery(false), shutBattery(false),
          lastVolume(BAT_VOLUME_NONE), adcValue(0), hasHintShutMessage(false),
          shutdownTimer(NULL)
    {}
    ~PowerMangerPrivate(){}

    PowerManger * const q_ptr;
    PowerSuplyType powerType;       // 电源类型
    bool lowBattery;                // 低电量
    bool shutBattery;               // 关机电量
    BatteryPowerLevel lastVolume;   // 上一次的电量等级
    short adcValue;                 // 电池电量
    bool hasHintShutMessage;        // 是否弹出过关机提示
    QTimer *shutdownTimer;

    void monitorRun();

    /**
     * @brief initBatteryData 初始化电池属性
     */
    void initBatteryData(void);

    /**
     * @brief getCurrentVolume 获得当前电量等级
     * @return
     */
    BatteryPowerLevel getCurrentVolume(void);

    /**
     * @brief updateBatAlarm 更新电池报警
     * @param type
     * @param isOccure 是否发生，true:发生，false:移除
     */
    void updateBatAlarm(BatOneShotType type, bool isOccure);

    /**
     * @brief hasBattery 返回是否有电池接入。
     * @return
     */
    bool hasBattery(void);

    void shutdownWarn(void);

    QList<BatteryPowerLevel> powerList;
};

PowerManger::~PowerManger()
{
    delete d_ptr;
}

void PowerManger::setBatteryCapacity(short adc)
{
    d_ptr->adcValue = adc;
}

QString PowerManger::getBatteryCapacity()
{
    QString batQuantityStr;
    if (d_ptr->powerType == POWER_SUPLY_AC_BAT || d_ptr->powerType == POWER_SUPLY_BAT)
    {
        float batQuantity = ((d_ptr->adcValue - BAT_LEVEL_0) * 1.0 ) / (BAT_LEVEL_5 - BAT_LEVEL_0) * 100;
        if (batQuantity < 0)
        {
            batQuantity = 0;
        }
        else if (batQuantity > 100)
        {
            batQuantity = 100;
        }
        batQuantityStr = QString("%1%2").arg(QString::number(batQuantity, 'f', 1), "%");
    }
    else
    {
        batQuantityStr = "-";
    }
    return batQuantityStr;
}

void PowerManger::run()
{
    d_ptr->monitorRun();
}

void PowerManger::powerOff()
{
    systemBoardProvider.requestShutdown();
}

PowerManger::PowerManger()
    : QObject(),
      d_ptr(new PowerMangerPrivate(this))
{
    // 发送指令请求下位机上传电池状态
    systemBoardProvider.queryBatteryInfo();
    d_ptr->shutdownTimer = new QTimer();
    d_ptr->shutdownTimer->setInterval(TWO_MINUTE);
    connect(d_ptr->shutdownTimer, SIGNAL(timeout()), this, SLOT(powerOff()));
    d_ptr->powerList.clear();
    for (int i = 0; i < POWER_LIST_MAX_COUNT; i++)
    {
        d_ptr->powerList.append(BAT_VOLUME_NONE);
    }
}

void PowerMangerPrivate::monitorRun()
{
    powerType = systemBoardProvider.getPowerSuplyType();

    // ******控制电量图标显示**********
    if (powerType == POWER_SUPLY_AC || powerType == POWER_SUPLY_UNKOWN)
    {
        batteryBarWidget.setStatus(BATTERY_NOT_EXISTED);
        initBatteryData();
        shutdownTimer->stop();
    }
    else if (powerType == POWER_SUPLY_AC_BAT)
    {
        if (adcValue == 0)
        {
            // 不处理无效值
            return;
        }
        BatteryPowerLevel curVolume = getCurrentVolume();
        batteryBarWidget.setStatus(BATTERY_CHARGING);
        batteryBarWidget.setIcon(curVolume);
        if (curVolume != BAT_VOLUME_5 && systemBoardProvider.isPowerCharging())
        {
            batteryBarWidget.charging();
        }
        shutdownTimer->stop();
        shutBattery = false;
        lowBattery = false;
    }
    else if (powerType == POWER_SUPLY_BAT)
    {
        batteryBarWidget.setStatus(BATTERY_NORMAL);
        BatteryPowerLevel curVolume = getCurrentVolume();

        // 连续三次都是一样的结果才确定结果
        powerList.removeFirst();
        powerList.append(curVolume);
        for (int i = 0; i < powerList.count(); i++)
        {
            if (curVolume != powerList.at(i) && powerList.at(i) != BAT_VOLUME_NONE)
            {
                curVolume = powerList.at(i);
            }
        }
        shutBattery = false;
        lowBattery = false;
        if (curVolume == BAT_VOLUME_0)
        {
            shutBattery = true;
            lowBattery = true;
        }
        else if (curVolume == BAT_VOLUME_1)
        {
            lowBattery = true;
        }

        batteryBarWidget.setIcon(curVolume);
        if (shutBattery)
        {
            // 是否电量低于开机所需电量，是则自动关机
            shutdownWarn();
        }
        else if (lowBattery)
        {
            // 电量到达低电量，提示电量过低
            batteryBarWidget.setIconLow();  // 黄灯显示低电量
            if (lastVolume != curVolume)
            {
                MessageBox lowMessage(trs("Prompt"), trs("LowBattery"), false);
                windowManager.showWindow(&lowMessage, WindowManager::ShowBehaviorCloseIfVisiable
                                         | WindowManager::ShowBehaviorModal);
            }
            shutdownTimer->stop();
        }
        else
        {
            shutdownTimer->stop();
        }
        lastVolume = curVolume;
    }
    else
    {
        initBatteryData();
    }
    // *********************************

    // ********控制报警栏显示***********
    updateBatAlarm(BAT_ONESHOT_LOW_BATTERY, lowBattery);
    updateBatAlarm(BAT_ONESHOT_NO_BATTERY, !hasBattery());
    //**********************************
}

void PowerMangerPrivate::initBatteryData()
{
    lowBattery = false;
    shutBattery = false;
}

BatteryPowerLevel PowerMangerPrivate::getCurrentVolume()
{
    short batteryADCVoltage = 0;          // 记录ADC电压

    if (adcValue < 500)
    {
        return BAT_VOLUME_NONE;
    }
    batteryADCVoltage = adcValue;

    BatteryPowerLevel powerLevel = BAT_VOLUME_0;
    if (batteryADCVoltage < BAT_LEVEL_0)
    {
        // over low
        powerLevel = BAT_VOLUME_0;
    }
    else if (batteryADCVoltage >= BAT_LEVEL_0 && batteryADCVoltage < BAT_LEVEL_1)
    {
        powerLevel = BAT_VOLUME_1;
    }
    else if (batteryADCVoltage >= BAT_LEVEL_1 && batteryADCVoltage < BAT_LEVEL_2)
    {
        powerLevel = BAT_VOLUME_2;
    }
    else if (batteryADCVoltage >= BAT_LEVEL_2 && batteryADCVoltage < BAT_LEVEL_3)
    {
        powerLevel = BAT_VOLUME_3;
    }
    else if (batteryADCVoltage >= BAT_LEVEL_3 && batteryADCVoltage < BAT_LEVEL_4)
    {
        powerLevel = BAT_VOLUME_4;
    }
    else if (batteryADCVoltage >= BAT_LEVEL_4)
    {
        powerLevel = BAT_VOLUME_5;
    }

    return powerLevel;
}

void PowerMangerPrivate::updateBatAlarm(BatOneShotType type, bool isOccure)
{
    batteryOneShotAlarm.setOneShotAlarm(type, isOccure);
}

bool PowerMangerPrivate::hasBattery()
{
    if ((systemBoardProvider.getPowerSuplyType() == POWER_SUPLY_BAT) ||
            (systemBoardProvider.getPowerSuplyType() == POWER_SUPLY_AC_BAT))
    {
        return true;
    }

    return false;
}

void PowerMangerPrivate::shutdownWarn()
{
    if (hasHintShutMessage)
    {
        return;
    }

    if (systemBoardProvider.getPowerSuplyType() != POWER_SUPLY_AC
            && systemBoardProvider.getPowerSuplyType() != POWER_SUPLY_AC_BAT)
    {
        shutdownTimer->start();
        hasHintShutMessage = true;
        // 清除界面弹出框
        windowManager.closeAllWidows();
        MessageBox lowMessage(trs("Prompt"), trs("shutDownIn2Min"), false, true);
        windowManager.showWindow(&lowMessage, WindowManager::ShowBehaviorNoAutoClose |
                                 WindowManager::ShowBehaviorModal);
    }
}
