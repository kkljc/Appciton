#pragma once

#include <QObject>
#include <fstream>
#include <QString>
#include <QMap>
#include "XmlParser.h"

class XmlParser;
class LanguageManager
{
public:

    enum LanguageName {
        English = 0,
        Chinese_Simplify,
        //Chinese_Traditional_TW,
        //Chinese_Traditional_HK,
        Language_NR
    };

    static LanguageManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new LanguageManager();
        }
        return *_selfObj;
    }
    static LanguageManager *_selfObj;
    ~LanguageManager();

public:
    const QString &translate(const char *id);
    const QString &translate(const QString &id);
    const QString &translatePrint(const char *id);
    const QString &translatePrint(const QString &id);
    LanguageName getCurLanguage() const;
    void reload(int index);
    QString getCurVoicePromptFolderName() const;

private:
    LanguageManager();

    XmlParser _xmlParser;
    typedef QMap<QString, QString> LanguageMap;
    LanguageMap _languageMap;    // 运行期间存放翻译好字符串的容器。
    LanguageName _curLanguage;

    XmlParser _xmlPrintParser;
    LanguageMap _printLanguageMap; // 运行期间存放翻译好的打印字符串的容器。

#ifdef CONFIG_UNIT_TEST
    friend class TestLanguageManager;
#endif
};
#define languageManager (LanguageManager::construction())
#define deleteLanguageManager() (delete LanguageManager::_selfObj)

#define trs(str) languageManager.translate(str)
//#define ptrs(str) languageManager.translatePrint(str)
