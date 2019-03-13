/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/

#include "SystemManagerInterface.h"
#include "gmock/gmock.h"

class MockSystemManager : public SystemManagerInterface
{
public:
    MOCK_METHOD1(setBrightness, void(BrightnessLevel));
    MOCK_METHOD1(enableBrightness, void(BrightnessLevel));
    MOCK_METHOD0(getBrightness, BrightnessLevel(void));
};
