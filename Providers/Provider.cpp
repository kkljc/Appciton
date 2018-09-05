/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/5
 **/

#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <QVector>
#include "Debug.h"
#include "Provider.h"
#include "IConfig.h"

#ifdef Q_WS_X11
#include "UartSocket.h"
#endif

/**************************************************************************************************
 * 功能： 初始化端口。
 * 参数：
 *      desc： 属性描述。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Provider::initPort(const UartAttrDesc &desc, bool needNotify)
{
    QString port;
    machineConfig.getStrValue(_name + "Port", port);

    disPatchInfo.dispatcher = DataDispatcher::getDataDispatcher(port);
    if (disPatchInfo.dispatcher)
    {
        debug("%s connect to data dispatcher(%s), datatype %d",
              qPrintable(_name),
              qPrintable(port),
              disPatchInfo.packetType);
        disPatchInfo.dispatcher->connectProvider(disPatchInfo.packetType, this);
        return true;
    }

    debug("%s", qPrintable(_name));
    debug("%s", qPrintable(port));
    return uart->initPort(port, desc, needNotify);
}

/**************************************************************************************************
 * 功能： 写数据到端口。
 *************************************************************************************************/
int Provider::writeData(const unsigned char buff[], int len)
{
    // send through the dispatcher
    if (disPatchInfo.packetType != DataDispatcher::PACKET_TYPE_INVALID)
    {
        return disPatchInfo.dispatcher->sendData(disPatchInfo.packetType, buff, len);
    }

    return uart->write(buff, len);
}

/**************************************************************************************************
 * 功能： 读取数据到RingBuff中。
 *************************************************************************************************/
void Provider::readData(void)
{
    unsigned char buf[1024];
    int ret = uart->read(buf, sizeof(buf));
    if (ret < 0)
    {
//        debug("Data read error!\n");
        return;
    }

    ringBuff.push(buf, ret);

//    //血氧值、脉率值模拟--nellcor
//    unsigned char bu[13]={
//        0x55, 0x02, 0x08, 'j', 0x06,
//        0x80, 98, 0x80, 60, 0x50,
//        0x01, 46, 0x03
//    };
//    //棒图值、beep音模拟--nellcor
//    unsigned char bu[9]={
//        0x55, 0x02, 0x04, '~',
//        0x02, 1, 97, 13, 0x03
//    };
//    ringBuff.push(bu, 9);
}

/**************************************************************************************************
 * 功能： 获取名称。
 * 返回： Provider的名称。
 *************************************************************************************************/
QString &Provider::getName(void)
{
    return _name;
}

/**************************************************************************************************
 * 功能： 关联Provider与Param的接口。
 * 返回：如果失败则返回false。
 *************************************************************************************************/
bool Provider::attachParam(Param &param)
{
    param.getName();  // 预防编译警告。
    return true;
}

/**************************************************************************************************
 * 功能： 关联Provider与Param的接口。
 *************************************************************************************************/
void Provider::detachParam(Param &param)
{
    param.getName();
}

/**************************************************************************************************
 * 功能： 检查连接状态。
 *************************************************************************************************/
void Provider::checkConnection(void)
{
    _disconnectCount++;
    if (_disconnectCount > _disconnectThreshold)
    {
        _disconnectCount = 0;
        if (isConnected || _firstCheck)
        {
            _firstCheck = false;
            isConnected = false;
            disconnected();
        }
    }
}

/**************************************************************************************************
 * 关闭串口。
 *************************************************************************************************/
void Provider::closePort()
{
    uart->closePort();
}

/**************************************************************************************************
 * 是否连接。
 *************************************************************************************************/
bool Provider::connected()
{
    return isConnected;
}

/**************************************************************************************************
 * 功能：设置连接判断的限制。
 *************************************************************************************************/
void Provider::setDisconnectThreshold(int second)
{
    _disconnectThreshold = second;
}

/**************************************************************************************************
 * 功能： 有数据时调用清除连接计数器。
 *************************************************************************************************/
void Provider::feed(void)
{
    if (!isConnected)
    {
        reconnected();
    }
    isConnected = true;
    _disconnectCount = 0;
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
Provider::Provider(const QString &name) : QObject(), ringBuff(ringBuffLen), _name(name)
{
    _disconnectCount = 0;
    _disconnectThreshold = 5;
    isConnected = false;
    _firstCheck = true;
// #ifdef Q_WS_X11
//    uart = new UartSocket();
// #else
    uart = new Uart();
// #endif
    connect(uart, SIGNAL(activated(int)), this, SLOT(dataArrived()));
    uart->setParent(this);
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
Provider::~Provider()
{
    disconnect(uart, SIGNAL(activated(int)), this, SLOT(dataArrived()));
}
