#include "networkmanager.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QDebug>

class NetworkManager::Private
{
public:
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

QAbstractSocket *NetworkManager::createConnection(const TestDefinitionPtr &testDefinition, SocketType socketType)
{
    QAbstractSocket* socket = d->createSocket(socketType);
    if ( !socket ) {
        qDebug() << "Unknown socket type requested";
        return NULL;
    }

    // TODO: Implementation

    return NULL;
}

