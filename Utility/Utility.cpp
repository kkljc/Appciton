#include "Utility.h"
#include <time.h>
#include "Debug.h"
#include <QTimer>
#include <QEventLoop>
#include <QDir>
#include <QFileInfo>
#include <syslog.h>
#include <qglobal.h>

namespace Util
{
char *strlcpy(char *dest, const char *src, size_t n)
{
    size_t i;

    if(n == 0)
    {
        return NULL;
    }

    for (i = 0; i < n - 1 && src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }

    for ( ; i < n; i++)
    {
        dest[i] = '\0';
    }

    return dest;
}

quint64 systemBootTime()
{
    struct timespec tp;
    quint64 msec;
    if(clock_gettime(CLOCK_BOOTTIME, &tp))
    {
        debug("Failed to get boottime");
    }
    msec = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
    return msec;
}

void waitInEventLoop(int ms)
{
    QEventLoop eventloop;
    QTimer::singleShot(ms, &eventloop, SLOT(quit()));
    eventloop.exec();
}

void deleteDir(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        return;
    }

    foreach(QFileInfo fileinfo, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden | QDir::NoSymLinks | QDir::Files))
    {
        if (fileinfo.isDir())
        {
            deleteDir(fileinfo.filePath());
        }
        else
        {
            QFile::remove(fileinfo.filePath());
        }
    }

    dir.rmdir(path);
}


static void syslogMessageHandler(QtMsgType type, const char *msg)
{
     switch (type) {
     case QtDebugMsg:
         syslog(LOG_DEBUG, "%s",msg);
         break;
     case QtWarningMsg:
         syslog(LOG_WARNING, "%s",msg);
         break;
     case QtCriticalMsg:
         syslog(LOG_CRIT, "%s",msg);
         break;
     case QtFatalMsg:
         syslog(LOG_EMERG, "%s",msg);
     }
}

void setupSysLog()
{
    openlog("nPM", LOG_CONS|LOG_PID, 0);
    qInstallMsgHandler(syslogMessageHandler);
}

}