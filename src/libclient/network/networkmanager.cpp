#include "networkmanager.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QStringList>
#include <QMutex>
#include <QReadWriteLock>
#include <QDebug>

struct RemoteHost
{
    QString host;
    quint16 port;
};

class NetworkManager::Private
{
public:
    QMutex mutex;

    RemoteHost remoteHost(const QString& hostname) const;
    QAbstractSocket* createSocket(NetworkManager::SocketType socketType);
};

RemoteHost NetworkManager::Private::remoteHost(const QString &hostname) const
{
    RemoteHost host;

    if (hostname.contains('.')) {
        QStringList parts = hostname.split(':');
        host.host = parts.at(0);
        host.port = parts.at(1).toInt();
    } else {
        host.host = hostname;
        host.port = 1337;
    }

    return host;
}

QAbstractSocket *NetworkManager::Private::createSocket(NetworkManager::SocketType socketType)
{
    switch(socketType) {
    case TcpSocket:
        return new QTcpSocket;

    case UdpSocket:
        return new QUdpSocket;

//    case UtpSocket:
//        return new UtpSocket;

    default:
        return NULL;
    }
}

NetworkManager::NetworkManager(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

NetworkManager::~NetworkManager()
{
    delete d;
}

QAbstractSocket *NetworkManager::createConnection(const QString &hostname, NetworkManager::SocketType socketType)
{
    QAbstractSocket* socket = d->createSocket(socketType);
    if ( !socket ) {
        qDebug() << "Unknown socket type requested";
        return NULL;
    }

    RemoteHost remote = d->remoteHost(hostname);
    socket->connectToHost(remote.host, remote.port);

    return socket;
}

QAbstractSocket *NetworkManager::createConnection(const TestDefinitionPtr &testDefinition, SocketType socketType)
{
    QAbstractSocket* socket = d->createSocket(socketType);
    if ( !socket ) {
        qDebug() << "Unknown socket type requested";
        return NULL;
    }

    // TODO: Implementation

    return socket;
}

