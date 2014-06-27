#include "measurement.h"

#include <QPointer>
#include <QAbstractSocket>

class Measurement::Private
{
public:
    QPointer<QAbstractSocket> peerSocket;
    QUuid taskId;
    QUuid measurementUuid;
    QDateTime startDateTime;
    QString errorString;
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
QUuid Measurement::taskId() const
{
    return d->taskId;
}

void Measurement::setTaskId(const QUuid &taskId)
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

QString Measurement::errorString() const
{
    return d->errorString;
}

void Measurement::setErrorString(const QString &message)
{
    d->errorString = message;
}
