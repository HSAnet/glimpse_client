#include "wifilookup.h"
#include "../../log/logger.h"

LOGGER(DeviceInfo);

class WifiLookup::Private
{
public:
    Private()
    {
    }

};

WifiLookup::WifiLookup(QObject *parent)
: Measurement(parent)
, d(new Private)
, currentStatus(Unknown)
{
}

WifiLookup::~WifiLookup()
{
    delete d;
}

Measurement::Status WifiLookup::status() const
{
    return currentStatus;
}

void WifiLookup::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool WifiLookup::prepare(NetworkManager *networkManager,
                         const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<WifiLookupDefinition>();
    setErrorString("not implemented");
    return false;
}

bool WifiLookup::start()
{
    return false;
}

bool WifiLookup::stop()
{
    setStatus(WifiLookup::Finished);
    return true;
}

Result WifiLookup::result() const
{
    return Result(startDateTime(), endDateTime(), QVariantList());
}

