#include "measurement.h"

#include <QPointer>
#include <QAbstractSocket>

class Measurement::Private
{
public:
    QPointer<QAbstractSocket> peerSocket;
    TaskId taskId;
    QUuid measurementUuid;
    QDateTime startDateTime;
    QString errorString;
    QVariantMap preInfo;
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

TaskId Measurement::taskId() const
{
    return d->taskId;
}

void Measurement::setTaskId(const TaskId &taskId)
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

QVariantMap Measurement::preInfo() const
{
    return d->preInfo;
}

void Measurement::setPreInfo(const QVariantMap &preInfo)
{
    d->preInfo = preInfo;
}

QString Measurement::errorString() const
{
    return d->errorString;
}

void Measurement::setErrorString(const QString &message)
{
    d->errorString = message;
}
