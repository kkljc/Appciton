#pragma once
#include <QObject>
#include <QString>
#include <termios.h>
#include "Param.h"
#include "Uart.h"
#include "RingBuff.h"

class Provider: public QObject
{
    Q_OBJECT

public:
    // 获取Provider的名称。
    QString &getName(void);

    // 关联Provider和Param对象，具体内容由派生类实现。
    virtual bool attachParam(Param &param);

    // 脱离Provider和Param对象，具体内容由派生类实现。
    virtual void detachParam(Param &param);

    // 检查连接状态。
    void checkConnection(void);

    //获取版本号
    virtual void sendVersion(void) = 0;

     //关闭串口
    void closePort(void);

    // 是否连接
    bool connected();

    // 构造与析构。
    Provider(const QString &name);
    virtual ~Provider();

protected:
    // 初始化端口。
    bool initPort(const UartAttrDesc &desc, bool needNotify = true);

    // 写数据到端口。
    int writeData(const unsigned char buff[], int len);

    // 读取数据到RingBuff中。
    void readData(void);

    // 数据端口的描述符。
    RingBuff<unsigned char> ringBuff;
    static const int ringBuffLen = 4096;
    Uart *uart;

protected slots:
    // 当端口有数据时调用此方法，派生类实现。
    virtual void dataArrived(void)
    {
    }

protected:
    void setDisconnectThreshold(int second);
    void feed(void);                              // 有数据时调用清除连接计数器。
    virtual void disconnected(void) = 0;          // 模块连接恢复时回调，之类实现。
    virtual void reconnected(void) = 0;          // 模块连接恢复时回调，之类实现。
    bool isConnected;

private:
    QString _name;
    bool _firstCheck;

    int _timerID;
    int _disconnectCount;
    int _disconnectThreshold;
};