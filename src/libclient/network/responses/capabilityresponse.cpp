#include "capabilityresponse.h"
#include "log/logger.h"

LOGGER(CapabilityResponse);

class CapabilityResponse::Private
{
public:
    bool capabilitiesOK;
};
CapabilityResponse::CapabilityResponse(QObject *parent)
: Response(parent)
, d(new Private)
{

}

CapabilityResponse::~CapabilityResponse()
{
    delete d;
}

bool CapabilityResponse::fillFromVariant(const QVariantMap &variant)
{
    d->capabilitiesOK = true;

    QMapIterator<QString, QVariant> capabilityIter(variant);

    while (capabilityIter.hasNext())
    {
        capabilityIter.next();

        QVariantMap capabiltyResult = capabilityIter.value().toMap();

        if (capabiltyResult.value("registered", "") != "ok")
        {
            d->capabilitiesOK = false;
            LOG_ERROR(QString("Capability %1 could not be registered").arg(capabilityIter.key()))
        }
    }

    return d->capabilitiesOK;
}

