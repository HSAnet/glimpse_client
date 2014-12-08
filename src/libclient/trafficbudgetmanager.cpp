#include "client.h"
#include "settings.h"
#include "trafficbudgetmanager.h"
#include "network/networkmanager.h"
#include "log/logger.h"

#include <QReadLocker>
#include <QWriteLocker>

LOGGER(TrafficBudgetManager);


class TrafficBudgetManager::Private
{
public:
    quint32 availableTraffic;
    quint32 usedTraffic;
    quint32 availableMobileTraffic;
    quint32 usedMobileTraffic;
    QReadWriteLock lock;
    bool active;
};

TrafficBudgetManager::TrafficBudgetManager()
: d(new Private)
{
}

void TrafficBudgetManager::init()
{
    d->availableMobileTraffic = Client::instance()->settings()->availableMobileTraffic();
    d->availableTraffic = Client::instance()->settings()->availableTraffic();
    d->usedMobileTraffic = Client::instance()->settings()->usedMobileTraffic();
    d->usedTraffic = Client::instance()->settings()->usedTraffic();
    d->active = Client::instance()->settings()->trafficBudgetManagerActive();

    LOG_INFO(QString("Traffic budget manager is %1").arg(d->active ? "enabled" : "disabled"));

    if (d->active)
    {
        LOG_INFO(QString("Traffic: Used %1 of %2 MB").arg(d->usedTraffic/(1024*1024)).arg(d->availableTraffic/(1024*1024)));
        LOG_INFO(QString("Traffic (mobile): Used %1 of %2 MB").arg(d->usedMobileTraffic/(1024*1024)).arg(d->availableMobileTraffic/(1024*1024)));
    }
}

void TrafficBudgetManager::saveTraffic()
{
    if (Client::instance()->networkManager()->onMobileConnection())
    {
        Client::instance()->settings()->setAvailableMobileTraffic(d->availableMobileTraffic);
        Client::instance()->settings()->setUsedMobileTraffic(d->usedMobileTraffic);
    }
    else
    {
        Client::instance()->settings()->setAvailableTraffic(d->availableTraffic);
        Client::instance()->settings()->setUsedTraffic(d->usedTraffic);
    }
}

quint32 TrafficBudgetManager::availableTraffic() const
{
    QReadLocker locker(&d->lock);

    return Client::instance()->networkManager()->onMobileConnection() ? d->availableMobileTraffic :
           d->availableTraffic;
}

bool TrafficBudgetManager::addUsedTraffic(quint32 traffic)
{
    QWriteLocker locker(&d->lock);

    if (Client::instance()->networkManager()->onMobileConnection())
    {
        if (d->availableMobileTraffic >= traffic)
        {
            d->usedMobileTraffic += traffic;
            saveTraffic();
            return true;
        }
    }
    else
    {
        if (d->availableTraffic >= traffic)
        {
            d->usedTraffic += traffic;
            saveTraffic();
            return true;
        }
    }

    return !d->active;
}

quint32 TrafficBudgetManager::usedTraffic() const
{
    QReadLocker locker(&d->lock);

    return Client::instance()->networkManager()->onMobileConnection() ? d->usedMobileTraffic : d->usedTraffic;
}
