#include "networkmanager.h"
#include "../networkhelper.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QStringList>
#include <QMutex>
#include <QReadWriteLock>
#include <QDebug>

class NetworkManager::Private
{
public:
    QMutex mutex;

    QAbstractSocket* createSocket(NetworkManager::SocketType socketType);
};


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

    if (socketType != UdpSocket) {
        RemoteHost remote = NetworkHelper::remoteHost(hostname);
        socket->connectToHost(remote.host, remote.port);
        socket->waitForConnected();
    }

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

