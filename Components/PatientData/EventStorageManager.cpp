#include "EventStorageManager.h"
#include "StorageManager_p.h"
#include "StorageFile.h"
#include "DataStorageDirManager.h"
#include "EventStorageItem.h"
#include "ECGParam.h"
#include <QList>
#include <QMutex>
#include "Debug.h"
#include "WindowManager.h"

#define MAX_STORE_WAVE_NUM 3

class EventStorageManagerPrivate: public StorageManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(EventStorageManager)

    EventStorageManagerPrivate(EventStorageManager *q_ptr)
        :StorageManagerPrivate(q_ptr)
    {

    }


    QList<WaveformID> getStoreWaveList(WaveformID paramWave);

    QList<EventStorageItem *> eventItemList;
    QMutex mutex;
};

QList<WaveformID> EventStorageManagerPrivate::getStoreWaveList(WaveformID paramWave)
{
    QList<int> displaywaves;

    windowManager.getDisplayedWaveform(displaywaves);

    WaveformID calcLeadWave = (WaveformID)ecgParam.getCalcLead();


    QList<WaveformID> storeWaves;

    if(paramWave != WAVE_NONE)
    {
        storeWaves.append(paramWave);
    }

    if(paramWave != calcLeadWave)
    {
        storeWaves.append(calcLeadWave);
    }

    foreach(int id, displaywaves)
    {
        if(storeWaves.count() >= MAX_STORE_WAVE_NUM)
            break;

        if(id <= WAVE_ECG_V6 || id == paramWave)
        {
            continue;
        }

        storeWaves.append((WaveformID) id);
    }

    return storeWaves;
}

EventStorageManager &EventStorageManager::getInstance()
{
    static EventStorageManager *instance = NULL;
    if(instance == NULL)
    {
        instance = new EventStorageManager();
    }
    return *instance;
}

EventStorageManager::~EventStorageManager()
{

}

void EventStorageManager::triggerEvent(EventType type)
{
    Q_D(EventStorageManager);
    EventStorageItem *eventItem = NULL;
    switch(type)
    {
    case EventPhysiologicalAlarm:
        break;
    case EventCodeMarker:
        break;
    case EventRealtimePrint:
        break;
    case EventNIBPMeasurement:
        break;
    case EventWaveFreeze:
        break;
    default:
        qdebug("Unknown event type %d", type);
        return;
    }

    if(eventItem)
    {
        d->mutex.lock();
        d->eventItemList.append(eventItem);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerAlarmEvent(const AlarmInfoSegment &almInfo, WaveformID paramWave)
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventPhysiologicalAlarm,
                                                  d->getStoreWaveList(paramWave),
                                                  almInfo);
    if(item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerCodeMarkerEvent(const char *codeName)
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventCodeMarker,
                                                  d->getStoreWaveList(WAVE_NONE),
                                                  codeName);
    if(item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerRealtimePrintEvent()
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventRealtimePrint,
                                                  d->getStoreWaveList(WAVE_NONE));
    if(item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerNIBPMeasurementEvent()
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventNIBPMeasurement,
                                                  d->getStoreWaveList(WAVE_NONE));
    if(item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerWaveFreezeEvent()
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventWaveFreeze,
                                                  d->getStoreWaveList(WAVE_NONE));
    if(item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::checkCompletedEvent()
{
    Q_D(EventStorageManager);
    QMutexLocker locker(&d->mutex);
    while(!d->eventItemList.isEmpty())
    {
        EventStorageItem *item = d->eventItemList.first();

        if(item->checkCompleted())
        {
            d->eventItemList.takeFirst();

            addData(item->getType(), item->getStorageData());

            delete item;
        }
        else
        {
            break;
        }
    }
}


void EventStorageManager::run()
{
    if(dataStorageDirManager.isCurRescueFolderFull())
    {
        discardData();
        return;
    }

    dataStorageDirManager.addDataSize(saveData());
}

EventStorageManager::EventStorageManager()
    :StorageManager(new EventStorageManagerPrivate(this), new StorageFile())
{
    Q_D(EventStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + EVENT_DATA_FILE_NAME, QIODevice::ReadWrite);
}