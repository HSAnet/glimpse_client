#include "measurement.h"

#include <QPointer>
#include <QAbstractSocket>

class Measurement::Private
{
public:
    Private()
    {
        taskId = QUuid();
    }

    QPointer<QAbstractSocket> peerSocket;
    QUuid taskId;
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
