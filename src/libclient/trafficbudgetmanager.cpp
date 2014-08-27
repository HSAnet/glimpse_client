#include "client.h"
#include "settings.h"
#include "trafficbudgetmanager.h"
#include "network/networkmanager.h"

class TrafficBudgetManager::Private
{
public:
    quint32 availableTraffic;
    quint32 usedTraffic;
    quint32 availableMobileTraffic;
    quint32 usedMobileTraffic;
    bool onMobileConnection;
};

TrafficBudgetManager::TrafficBudgetManager()
{
    d->onMobileConnection = Client::instance()->networkManager()->onMobileConnection();

    if (d->onMobileConnection)
    {
        setAvailableTraffic(Client::instance()->settings()->availableMobileTraffic());
    }
    else
    {
        setAvailableTraffic(Client::instance()->settings()->availableTraffic());
    }
}

TrafficBudgetManager::~TrafficBudgetManager()
{
    if (d->onMobileConnection)
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

void TrafficBudgetManager::setAvailableTraffic(quint32 traffic)
{
    if (d->onMobileConnection)
    {
        d->availableMobileTraffic = traffic;
    }
    else
    {
        d->availableTraffic = traffic;
    }
}

quint32 TrafficBudgetManager::availableTraffic() const
{
    return d->onMobileConnection ? d->availableMobileTraffic :
           d->availableTraffic;
}

bool TrafficBudgetManager::addUsedTraffic(quint32 traffic)
{
    if (d->onMobileConnection)
    {
        if (d->availableMobileTraffic >= traffic)
        {
            d->usedMobileTraffic += traffic;
            return true;
        }
    }
    else
    {
        if (d->availableTraffic >= traffic)
        {
            d->usedTraffic += traffic;
            return true;
        }
    }

    return false;
}

quint32 TrafficBudgetManager::usedTraffic() const
{
    return d->onMobileConnection ? d->usedMobileTraffic : d->usedTraffic;
}
