#include "measurement.h"
#include "../client.h"
#include "../settings.h"
#include "../network/networkmanager.h"

#include <QPointer>
#include <QAbstractSocket>

class Measurement::Private
{
public:
    QPointer<QAbstractSocket> peerSocket;
    quint32 taskId;
    QUuid measurementUuid;
    QDateTime startDateTime;
    QDateTime endDateTime;
    QString errorString;
    QStringList resultHeader;
    quint32 availableTraffic;
    quint32 usedTraffic;
    quint32 availableMobileTraffic;
    quint32 usedMobileTraffic;
    bool onMobileConnection;
};

Measurement::Measurement(QObject *parent)
: QObject(parent)
, d(new Private)
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

Measurement::~Measurement()
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

    delete d;
}

void Measurement::setPeerSocket(QAbstractSocket *peerSocket)
{
    d->peerSocket = peerSocket;
}

QAbstractSocket *Measurement::peerSocket() const
{
    return d->peerSocket;
}
quint32 Measurement::taskId() const
{
    return d->taskId;
}

void Measurement::setTaskId(const quint32 &taskId)
{
    d->taskId = taskId;
}

QUuid Measurement::getMeasurementUuid() const
{
    return d->measurementUuid;
}

void Measurement::setMeasurementUuid(const QUuid &measurementUuid)
{
    d->measurementUuid = measurementUuid;
}

QDateTime Measurement::startDateTime() const
{
    return d->startDateTime;
}

void Measurement::setStartDateTime(const QDateTime &startDateTime)
{
    d->startDateTime = startDateTime;
}

QDateTime Measurement::endDateTime() const
{
    return d->endDateTime;
}

void Measurement::setEndDateTime(const QDateTime &endDateTime)
{
    d->endDateTime = endDateTime;
}

QString Measurement::errorString() const
{
    return d->errorString;
}

QStringList Measurement::resultHeader() const
{
    return d->resultHeader;
}

void Measurement::setResultHeader(const QStringList &columnLabels)
{
    d->resultHeader = columnLabels;
}

void Measurement::setErrorString(const QString &message)
{
    d->errorString = message;
}

void Measurement::setAvailableTraffic(quint32 traffic)
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

quint32 Measurement::availableTraffic() const
{
    return d->onMobileConnection ? d->availableMobileTraffic :
           d->availableTraffic;
}

bool Measurement::isTrafficAvailable(quint32 traffic) const
{
    return d->onMobileConnection ? d->availableMobileTraffic >= traffic :
           d->availableTraffic >= traffic;
}

void Measurement::addUsedTraffic(quint32 traffic)
{
    if (d->onMobileConnection)
    {
        d->usedMobileTraffic += traffic;
    }
    else
    {
        d->usedTraffic += traffic;
    }
}

quint32 Measurement::usedTraffic() const
{
    return d->onMobileConnection ? d->usedMobileTraffic : d->usedTraffic;
}
