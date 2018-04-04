#include <QDesktopWidget>
#include "IApplication.h"
#include "FloatHandle.h"
#include "Starter.h"
#include "Debug.h"
#include "Init.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"

static IThread *_storageThread = NULL;
static IThread *_printThread = NULL;
static IThread *_printExecThread = NULL;
static IThread *_audioThread = NULL;
static QThread *_networkThread = NULL;

/**************************************************************************************************
 * 功能： 周期性任务，1秒执行一次。
 *************************************************************************************************/
static void _taskOneSec1(void)
{
    paramManager.checkProviderConnection();
    systemBoardProvider.checkConnection();
}

/**************************************************************************************************
 * 功能： 周期性任务，1s执行一次。
 *************************************************************************************************/
static void _taskOneSec2(void)
{
    summaryStorageManager.checkCompletedItem();
    eventStorageManager.checkCompletedEvent();
    powerManager.run();

    if (!usbManager.isUSBExist())
    {
        sysStatusBar.hideIcon(SYSTEM_ICON_LABEL_USB);
    }
    else
    {
        sysStatusBar.changeIcon(SYSTEM_ICON_LABEL_USB, SYSTEM_ICON_USB_CONNECTED);
    }
}

/**************************************************************************************************
 * 功能： 周期性任务，500ms执行一次。
 *************************************************************************************************/
static void _task500MSec0(void)
{
    // 更新界面显示。
    static unsigned t = 0;
    if (0 != t)
    {
        double diff_t = timeDate.difftime(timeDate.time(), t);
        t += (int)diff_t;
        if (isZero(diff_t))
        {
            return;
        }
    }
    else
    {
        t = timeDate.time();
    }

    timeManager.mainRun(t);
}

/**************************************************************************************************
 * 功能： 周期性任务，500ms执行一次。
 *************************************************************************************************/
static void _task500MSec1(void)
{
    nibpParam.machineRun();
}

/**************************************************************************************************
 * 功能： 周期性任务，50ms执行一次。
 *
 *************************************************************************************************/
static void _task50MSec(void)
{
    static unsigned t = 0;
    if(t == 0)
    {
        t = timeManager.getCurTime();
    }

    if (timeManager.getCurTime() < t + 1)
    {
        return;
    }

    // 数据收集
    trendCache.collectTrendData(t);

    /*
     * The paramDataStorageManager should run as follow order
     * store nibp measurement, handle alarm, handle 30 seconds trend
     */

    paramDataStorageManager.addNIBPData(t);

    // 报警处理
    alertor.mainRun(t);

    //collect alarm status
    trendCache.collectTrendAlarmStatus(t);

    trendDataStorageManager.periodRun(t);
    paramManager.showSubParamValue();

    t++;
}

/**************************************************************************************************
 * 存储线程。
 *************************************************************************************************/
static void _storageThreadEntry(void)
{
    // 保存配置。
    systemConfig.saveToDisk();
    superRunConfig.saveToDisk();

    static int loopCounter = 0;
    loopCounter ++;

    // 数据存储
    static bool cleanup = true;
    if(cleanup)
    {
        DataStorageDirManager::cleanUpLastIncidentDir();
        cleanup = false;
    }

    if(!systemManager.isGoingToTrunOff())
    {
        trendDataStorageManager.run();
    }

    if(!systemManager.isGoingToTrunOff())
    {
        summaryStorageManager.run();
    }

    if(!systemManager.isGoingToTrunOff())
    {
        eventStorageManager.run();
    }

    if(!systemManager.isGoingToTrunOff())
    {
        ecg12LDataStorage.run();
    }

    if(!systemManager.isGoingToTrunOff())
    {
        rawDataCollection.run();
        rawDataCollectionTxt.run();
    }

    if(!systemManager.isGoingToTrunOff())
    {
        errorLog.run();
    }


    //DisclosureItem::testPatienData();
    //DisclosureItem::testDeviceConfiguration();
    //DisclosureItem::testModuleConfig();
    //DisclosureItem::testAlarmStatus();
    //DisclosureItem::testAlarmEvent();
    //DisclosureItem::testTimestampMessage();
    //DisclosureItem::testDataChange();
    //DisclosureItem::testContinuousWave();
    //DisclosureItem::testSystemStatus();
    //DisclosureItem::testDefibState();
    //DisclosureItem::testDefibFireEvent();
    //DisclosureItem::testPaceEvent();
    //DisclosureItem::testCprCompression();
    //DisclosureItem::testCprAccelWaveform();
    //DisclosureItem::testForegroundAnalysis();
    //DisclosureItem::testBackgroundAnalysis();
    //DisclosureItem::testCodeMarker();
    //DisclosureItem::test12LeadEcgSnapshot();

    IThread::msleep(300);
}

/**************************************************************************************************
 * 打印线程。
 *************************************************************************************************/
static void _printThreadEntry(void)
{
    printManager.threadRun();
    IThread::msleep(5);
}

/**************************************************************************************************
 * 打印执行线程。
 *************************************************************************************************/
static void _printExecThreadEntry(void)
{
    printExec.run();
    IThread::msleep(5); // 5毫秒的间隔能够满足50mm/s的打印速度。
}

/**************************************************************************************************
 * 语音播放线程。
 *************************************************************************************************/
static void _audioThreadEntry(void)
{
    soundManager.run();
//    IThread::usleep(200);
    IThread::msleep(1);
}

/**************************************************************************************************
 * 功能： 初始化任务。
 *************************************************************************************************/
static void _initTasks(void)
{
    systemTick.addHook(500, 400, _task500MSec0);
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        systemTick.addHook(500, 200, _task500MSec1);
    }
    systemTick.addHook(50, 0, _task50MSec);

//    systemTick.addHook(1000, 0, _taskOneSec0);
    systemTick.addHook(1000, 100, _taskOneSec1);
    systemTick.addHook(1000, 200, _taskOneSec2);

    // 初始化所有的线程。
    _storageThread = new IThread("storage", _storageThreadEntry);
    _printThread = new IThread("print", _printThreadEntry);
    _printExecThread = new IThread("printExec", _printExecThreadEntry);
    _audioThread = new IThread("audio", _audioThreadEntry);

    _networkThread = new QThread();
    _networkThread->setObjectName("Network");
    networkManager.moveToThread(_networkThread);
    mailManager.moveToThread(_networkThread);
    QObject::connect(_networkThread, SIGNAL(finished()), &networkManager, SLOT(deleteLater()));
    QObject::connect(_networkThread, SIGNAL(finished()), &mailManager, SLOT(deleteLater()));
}

/**************************************************************************************************
 * 功能： 启动。
 *************************************************************************************************/
static void _start(void)
{
    systemTick.startTick();

    _storageThread->start();
    _printThread->start();
    _printExecThread->start();
    _audioThread->start();
    _networkThread->start();
    _networkThread->setPriority(QThread::IdlePriority);

    systemManager.loadInitBMode();
}

/**************************************************************************************************
 * 功能： 停止。
 *************************************************************************************************/
static void _stop(void)
{
    // 停止打印。
    printManager.abort();

    _storageThread->stop();
    _printExecThread->stop();
    _printThread->stop();
    _audioThread->stop();
    _networkThread->quit();

    // 删除线程对象。
    delete _storageThread;
    delete _printExecThread;
    delete _printThread;
    delete _audioThread;
    //network thread might be still running, need to delete later
    _networkThread->deleteLater();
}

/**************************************************************************************************
 * 功能： 主函数入口。
 *************************************************************************************************/
void appMain(IApplication &app)
{
    // 创建对象。
    newObjects();
    _initTasks();

    _start();            // 启动子系统。
    app.exec();          // 执行主循环。
    _stop();             // 停止子系统。

    // 退出前释放创建的对象。
    deleteObjects();
}