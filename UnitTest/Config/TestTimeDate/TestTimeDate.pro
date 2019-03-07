QT       += testlib

QT       -= gui

TARGET = testtimedate
CONFIG += qt console warn_on depend_includepath testcase
CONFIG   -= app_bundle

include("../../Common/Common.pri")
include("../../Common/googletest.pri")

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        testtimedate.cpp \
    ../../../Utility/Config/XmlParser.cpp \
    ../../../Utility/ErrorLog/ErrorLogInterface.cpp \
    ../../../Utility/ErrorLog/ErrorLogItem.cpp \
    ../../../Utility/TimeDate/TimeDate.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Utility/Config/Config.cpp \
    main.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    testtimedate.h \
    ../../../Utility/Config/XmlParser.h \
    ../../../Utility/ErrorLog/ErrorLogInterface.h \
    ../../../Utility/ErrorLog/ErrorLogItem.h \
    ../../../Utility/TimeDate/TimeDate.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Utility/Config/Config.h \
    ConfigManager.h

INCLUDEPATH += \
    ../../../Utility/ErrorLog \
    ../../../Utility/TimeDate \
    ../../../Utility/Config \
