#include "networkmanager.h"
#include "../networkhelper.h"

#include "tcpsocket.h"
#include "udpsocket.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QStringList>
#include <QHash>
#include <QMutex>
#include <QReadWriteLock>
#include <QDebug>

class NetworkManager::Private : public QObject
{
    Q_OBJECT

public:
    // Properties
    QMutex mutex;
    QHash<QString, QObject*> objectHash;
    QHash<QString, QAbstractSocket*> socketHash;

    // Functions
    QAbstractSocket* createSocket(NetworkManager::SocketType socketType);

public slots:
    void socketDestroyed(QObject* obj);
};

QAbstractSocket *NetworkManager::Private::createSocket(NetworkManager::SocketType socketType)
{
    QAbstractSocket* socket = NULL;

    switch(socketType) {
    case TcpSocket:
        socket = new TcpSocket;
        break;

    case UdpSocket:
        socket = new UdpSocket;
        break;

    case UtpSocket:
        //socket = new UtpSocket;
        break;

    default:
        break;
    }

    if (socket) {
        connect(socket, SIGNAL(destroyed(QObject*)), this, SLOT(socketDestroyed(QObject*)));
    }

    return socket;
}

void NetworkManager::Private::socketDestroyed(QObject *obj)
{
    QString hostname = objectHash.key(obj);
    if (hostname.isEmpty())
        return;

    objectHash.remove(hostname);
    socketHash.remove(hostname);
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

QAbstractSocket *NetworkManager::connection(const QString &hostname, NetworkManager::SocketType socketType) const
{
    return d->socketHash.value(hostname);
}

QAbstractSocket *NetworkManager::createConnection(const QString &hostname, NetworkManager::SocketType socketType)
{
    QAbstractSocket* socket = d->createSocket(socketType);
    if ( !socket ) {
        qDebug() << "Unknown socket type requested";
        return NULL;
    }

    d->socketHash.insert(hostname, socket);
    d->objectHash.insert(hostname, socket);

    // TODO: Create separate functions that connect the sockets and check them prior
    //       to returning them back to the caller
    /*if (socketType != UdpSocket) {
        RemoteHost remote = NetworkHelper::remoteHost(hostname);
        socket->connectToHost(remote.host, remote.port);
        socket->waitForConnected();
    }*/

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

QTcpServer *NetworkManager::createServerSocket()
{
    // TODO: Connect to traffic counter
    QTcpServer* server = new QTcpServer;
    return server;
}

#include "networkmanager.moc"
