#include "client.h"
#include "settings.h"
#include "trafficbudgetmanager.h"
#include "network/networkmanager.h"

#include <QReadLocker>
#include <QWriteLocker>

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
