#include "request.h"

class Request::Private
{
public:
    QUuid deviceId;
    QString sessionId;
};

Request::Request(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

Request::~Request()
{
    delete d;
}

void Request::setDeviceId(const QUuid& deviceId)
{
    if ( d->deviceId != deviceId ) {
        d->deviceId = deviceId;
        emit deviceIdChanged(deviceId);
    }
}

QUuid Request::deviceId() const
{
    return d->deviceId;
}

void Request::setSessionId(const QString &sessionId)
{
    if ( d->sessionId != sessionId ) {
        d->sessionId = sessionId;
        emit sessionIdChanged(sessionId);
    }
}

QString Request::sessionId() const
{
    return d->sessionId;
}
