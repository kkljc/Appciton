/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/4
 **/


#include <QCoreApplication>
#include <QTest>
#include "TestLanguageManager.h"
#include "TestTranslator.h"
#include "googletest.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    QCoreApplication a(argc, argv);
    TestTranslator testTranslator;
    int ret = QTest::qExec(&testTranslator, argc, argv);
    if (ret)
    {
        return ret;
    }
    TestLanguageManager tc;
    ret = QTest::qExec(&tc, argc, argv);
    return ret;
}
