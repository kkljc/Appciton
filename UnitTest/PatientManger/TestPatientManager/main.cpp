/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/13
 **/


#include <QCoreApplication>
#include "TestPatientManager.h"
#include "googletest.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleMock(&argc, argv);
    QCoreApplication a(argc, argv);
    TestPatientManager tc;
    return QTest::qExec(&tc, argc, argv);
}
