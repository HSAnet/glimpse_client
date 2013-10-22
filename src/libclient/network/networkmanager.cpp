#include "networkmanager.h"
#include "../networkhelper.h"
#include "../log/logger.h"
#include "../scheduler/scheduler.h"
#include "../settings.h"
#include "../timing/immediatetiming.h"

#include "tcpsocket.h"
#include "udpsocket.h"

#include <QDataStream>
#include <QJsonDocument>
#include <QPointer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QStringList>
#include <QHash>
#include <QMutex>
#include <QReadWriteLock>
#include <QTimer>
#include <QDebug>

LOGGER(NetworkManager)

class PeerRequest
{
public:
    QVariant toVariant() {
        QVariantMap map;
        map.insert("task_id", taskId);
        map.insert("definition", measurementDefinition);
        map.insert("measurement", measurement);
        map.insert("peer", peer);
        map.insert("port", port);
        map.insert("protocol", protocol);
        return map;
    }

    static PeerRequest fromVariant(const QVariant& variant) {
        QVariantMap map = variant.toMap();

        PeerRequest request;
        request.taskId = map.value("task_id").toUuid();
        request.measurementDefinition = map.value("definition");
        request.measurement = map.value("measurement").toString();
        request.peer = map.value("peer").toString();
        request.port = map.value("port").toUInt();
        request.protocol = (NetworkManager::SocketType)map.value("protocol").toInt();
        return request;
    }

    QUuid taskId;
    QVariant measurementDefinition;
    QString measurement;
    QString peer;
    quint16 port;
    NetworkManager::SocketType protocol;
};

class NetworkManager::Private : public QObject
{
    Q_OBJECT

public:
    Private(NetworkManager* q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

    NetworkManager* q;

    // Properties
    QMutex mutex;
    QHash<QString, QObject*> objectHash;
    QHash<QString, QAbstractSocket*> socketHash;

    QTimer timer;
    QPointer<QUdpSocket> socket;
    QPointer<Scheduler> scheduler;
    QPointer<Settings> settings;

    quint16 localPort;

    // Functions
    QAbstractSocket* createSocket(NetworkManager::SocketType socketType);

    void updateSocket();
    void updateTimer();
    void processDatagram(const QByteArray& datagram, const QHostAddress& host, quint16 port);

public slots:
    void socketDestroyed(QObject* obj);
    void responseChanged();
    void timeout();
    void onDatagramReady();
};

QAbstractSocket *NetworkManager::Private::createSocket(NetworkManager::SocketType socketType)
{
    QAbstractSocket* socket = NULL;

    switch(socketType) {
    case TcpSocket:
        socket = new ::TcpSocket;
        break;

    case UdpSocket:
        socket = new ::UdpSocket;
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

void NetworkManager::Private::updateSocket()
{
    if (!socket.isNull()) {
        delete socket.data();
        socket.clear();
    }

    QString keepaliveAddress = settings->config()->keepaliveAddress();
    RemoteHost remote = NetworkHelper::remoteHost(keepaliveAddress);
    if (!remote.isValid())
        return;

    localPort = remote.port;

    socket = qobject_cast<QUdpSocket*>( q->createConnection(NetworkManager::UdpSocket) );
    connect(socket.data(), SIGNAL(readyRead()), this, SLOT(onDatagramReady()));
    if (!socket->bind(remote.port)) {
        LOG_ERROR(QString("Unable to bind port %1: %2").arg(remote.port).arg(socket->errorString()));
    }
}

void NetworkManager::Private::updateTimer()
{
    int interval = settings->config()->keepaliveSchedule()->interval();
    if (interval < 1000) {
        LOG_INFO("Keepalive interval < 1 sec will not be accepted.");
        return;
    } else {
        LOG_INFO(QString("Keepalive set to %1 sec.").arg(interval/1000));
    }

    timer.setInterval(interval);
    timer.start();
}

void NetworkManager::Private::processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
    QString hostAndPort = QString("%1:%2").arg(host.toString()).arg(port);
    LOG_DEBUG(QString("Received datagram from %1: %2").arg(hostAndPort).arg(QString::fromUtf8(datagram)));
//    if (settings->config()->keepaliveAddress() == hostAndPort) {
        // Master server
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(datagram, &error);

        if (error.error == QJsonParseError::NoError) {
            PeerRequest request = PeerRequest::fromVariant(document.toVariant());

            TimingPtr timing(new ImmediateTiming);
            TestDefinitionPtr testDefinition(new TestDefinition(request.taskId, request.measurement, timing, request.measurementDefinition));

            scheduler->enqueue(testDefinition);
        } else {
            LOG_ERROR(QString("Invalid JSon from master server: %1").arg(error.errorString()));
        }
//    } else {
        // TODO: Process incoming data
//    }
}

void NetworkManager::Private::responseChanged()
{
    updateSocket();
    updateTimer();

    // Send the first timeout now
    timeout();
}

void NetworkManager::Private::timeout()
{
    RemoteHost remote = NetworkHelper::remoteHost(settings->config()->keepaliveAddress());
    if (!remote.isValid()) {
        LOG_INFO("Invalid keepalive host");
        return;
    }

    QString sessionId = settings->sessionId();
    if (sessionId.isEmpty()) {
        LOG_INFO("Empty session id");
        return;
    }

    QStringList srcIp;
    srcIp.append( NetworkHelper::localIpAddress().toString() );

    QVariantMap map;
    map.insert("type", "keepalive");
    map.insert("session_id", sessionId);
    map.insert("src_ip", srcIp);
    map.insert("src_port", localPort);

    QByteArray data = QJsonDocument::fromVariant(map).toJson();
    socket->writeDatagram(data, QHostAddress(remote.host), remote.port);

    LOG_INFO("Alive packet sent");
}

void NetworkManager::Private::onDatagramReady()
{
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        QHostAddress host;
        quint16 port;
        socket->readDatagram(datagram.data(), datagram.size(), &host, &port);

        // Process the datagram
        processDatagram(datagram, host, port);
    }
}

NetworkManager::NetworkManager(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

NetworkManager::~NetworkManager()
{
    delete d;
}

bool NetworkManager::init(Scheduler* scheduler, Settings *settings)
{
    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(responseChanged()));

    d->scheduler = scheduler;
    d->settings = settings;
    d->responseChanged();

    return true;
}

void NetworkManager::setRunning(bool running)
{
    if (isRunning() == running)
        return;

    if ( !running )
        d->timer.stop();
    else
        d->updateTimer();
}

bool NetworkManager::isRunning() const
{
    return d->timer.isActive();
}

QAbstractSocket *NetworkManager::connection(const QString &hostname, NetworkManager::SocketType socketType) const
{
    return d->socketHash.value(hostname);
}

QAbstractSocket *NetworkManager::createConnection(NetworkManager::SocketType socketType)
{
    QAbstractSocket* socket = d->createSocket(socketType);
    if ( !socket ) {
        qDebug() << "Unknown socket type requested";
        return NULL;
    }

    //d->socketHash.insert(hostname, socket);
    //d->objectHash.insert(hostname, socket);

    return socket;
}

QAbstractSocket *NetworkManager::establishConnection(const QString &hostname, const QString &measurement, const QVariant &definition, NetworkManager::SocketType socketType)
{
    QAbstractSocket* socket = d->createSocket(socketType);
    if ( !socket ) {
        qDebug() << "Unknown socket type requested";
        return NULL;
    }

    RemoteHost remote = NetworkHelper::remoteHost(hostname);
    RemoteHost aliveRemote = NetworkHelper::remoteHost(d->settings->config()->keepaliveAddress());

    PeerRequest request;
    request.measurement = measurement;
    request.measurementDefinition = definition;
    request.peer = remote.host;
    request.port = aliveRemote.port;
    request.protocol = socketType;

    if ( socketType != TcpSocket ) {
        if (!socket->bind(remote.port)) {
            LOG_ERROR(QString("Unable to bind source port to %1").arg(remote.port));
            delete socket;
            return NULL;
        }

        QUdpSocket* udpSocket = qobject_cast<QUdpSocket*>(socket);
        udpSocket->writeDatagram(QByteArray(), QHostAddress(remote.host), remote.port);
    }

    QByteArray data = QJsonDocument::fromVariant(request.toVariant()).toJson();

    // Step one: Send test offer to peer directly
    d->socket->writeDatagram(data, QHostAddress(remote.host), d->localPort);

    // Step two: Send test offer to peer via alive-server
    d->socket->writeDatagram(data, QHostAddress(aliveRemote.host), aliveRemote.port);

    LOG_TRACE("Sent test offer to peer and alive-server");

    if ( socketType != UdpSocket ) {
        // Final step: Connect to remote host
        //if (socket->waitForReadyRead(5000))
        //    return socket;

        const int tries = 20;
        for(int i=0; i < tries; ++i) {
            socket->connectToHost(remote.host, remote.port);
            if (socket->waitForConnected(5000/tries))
                return socket;
        }

        LOG_ERROR(QString("Unable to connect tcp socket in %3 tries to %1: %2").arg(hostname).arg(socket->errorString()).arg(tries));
    } else {
        if (!socket->waitForReadyRead(5000))
            LOG_ERROR("Remote did not answer for 5 sec, aborting connection.");
    }

    delete socket;
    return NULL;
}

QTcpServer *NetworkManager::createServerSocket()
{
    // TODO: Connect to traffic counter
    QTcpServer* server = new QTcpServer;
    return server;
}

#include "networkmanager.moc"
