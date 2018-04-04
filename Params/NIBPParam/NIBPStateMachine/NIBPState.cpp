#include "NIBPState.h"

NIBPProviderIFace *NIBPState::provider = NULL;

/**************************************************************************************************
 * 设置超时并启动计时。
 *************************************************************************************************/
void NIBPState::setTimeOut(int timeOut)
{
    _timeOut = timeOut;
    _elapseTime.restart();
}

/**************************************************************************************************
 * 重新设置超时时限。
 *************************************************************************************************/
void NIBPState::resetTimeOut(int t)
{
    _timeOut = t;
}

/**************************************************************************************************
 * 增加倒计时时间。
 *************************************************************************************************/
void NIBPState::addTime(int t)
{
    _timeOut = _timeOut + t;
}

/**************************************************************************************************
 * 是否超时。
 *************************************************************************************************/
bool NIBPState::isTimeOut(void)
{
    if (_elapseTime.elapsed() > _timeOut)
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 获取时间流逝。
 *************************************************************************************************/
int NIBPState::elapseTime(void)
{
    return _elapseTime.elapsed();
}

/**************************************************************************************************
 * 获取状态对象的名字。
 *************************************************************************************************/
NIBPStateType NIBPState::type(void)
{
    return _type;
}

/**************************************************************************************************
 * 设置Provider对象。
 *************************************************************************************************/
void NIBPState::setProvider(NIBPProviderIFace *p)
{
    provider = p;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPState::NIBPState(NIBPStateType type)
{
    provider = NULL;
    _type = type;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPState::~NIBPState()
{

}