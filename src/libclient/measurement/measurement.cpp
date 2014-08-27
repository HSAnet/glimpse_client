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
};

Measurement::Measurement(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

Measurement::~Measurement()
{
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
