#include "networkmanager.h"
#include "../networkhelper.h"
#include "../log/logger.h"
#include "../scheduler/scheduler.h"
#include "../task/taskexecutor.h"
#include "../settings.h"
#include "../timing/immediatetiming.h"
#include "../timing/periodictiming.h"

#include "tcpsocket.h"
#include "udpsocket.h"

#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
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
#include <QDnsLookup>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkInterface>
#if defined(Q_OS_ANDROID)
#include <QAndroidJniObject>
#endif

LOGGER(NetworkManager);

class PeerRequest
{
public:
    QVariant toVariant()
    {
        QVariantMap map;
        map.insert("type", "peer_request");
        map.insert("measurementDefinition", measurementDefinition);
        map.insert("taskId", taskId.toInt());
        map.insert("measurementUuid", measurementUuid);
        map.insert("measurement", measurement);
        map.insert("peer", peer);
        map.insert("port", port);
        map.insert("protocol", protocol);
        return map;
    }

    static PeerRequest fromVariant(const QVariant &variant)
    {
        QVariantMap map = variant.toMap();

        PeerRequest request;
        request.measurementDefinition = map.value("measurementDefinition");
        request.taskId = TaskId(map.value("taskId").toInt());
        request.measurementUuid = map.value("measurementUuid").toUuid();
        request.measurement = map.value("measurement").toString();
        request.peer = map.value("peer").toString();
        request.port = map.value("port").toUInt();
        request.protocol = (NetworkManager::SocketType)map.value("protocol").toInt();
        return request;
    }

    QVariant measurementDefinition;
    TaskId taskId;
    QUuid measurementUuid;
    QString measurement;
    QString peer;
    quint16 port;
    NetworkManager::SocketType protocol;
};

class NetworkManager::Private : public QObject
{
    Q_OBJECT

public:
    Private(NetworkManager *q)
    : q(q)
#if defined(Q_OS_ANDROID)
    , networkInfo("de/hsaugsburg/informatik/mplane/NetInfo")
#endif
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
        connect(&keepaliveAddressLookup, SIGNAL(finished()), this, SLOT(lookupFinished()));

        keepaliveAddressLookup.setType(QDnsLookup::A);
    }

    NetworkManager *q;

    // Properties
    QMutex mutex;
    QHash<QString, QObject *> objectHash;
    QHash<QString, QAbstractSocket *> socketHash;

    QTimer timer;
    QPointer<QUdpSocket> socket;
    QPointer<Scheduler> scheduler;
    QPointer<Settings> settings;

    QSet<QUuid> handledMeasureUuids;

    quint16 localPort;

    RemoteHost keepaliveHost;
    QHostAddress keepaliveAddress;
    QDnsLookup keepaliveAddressLookup;

    QNetworkConfigurationManager ncm;
#if defined(Q_OS_ANDROID)
    QAndroidJniObject networkInfo;
#elif defined(Q_OS_IOS)
#else
    QNetworkInfo networkInfo;
#endif

    // Functions
    QAbstractSocket *createSocket(NetworkManager::SocketType socketType);

    void updateSocket();
    void updateTimer();
    void processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port);

public slots:
    void socketDestroyed(QObject *obj);
    void responseChanged();
    void timeout();
    void onDatagramReady();
    void lookupFinished();
};

QAbstractSocket *NetworkManager::Private::createSocket(NetworkManager::SocketType socketType)
{
    QAbstractSocket *socket = NULL;

    switch (socketType)
    {
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

    if (socket)
    {
        connect(socket, SIGNAL(destroyed(QObject *)), this, SLOT(socketDestroyed(QObject *)));
    }

    return socket;
}

void NetworkManager::Private::socketDestroyed(QObject *obj)
{
    QString hostname = objectHash.key(obj);

    if (hostname.isEmpty())
    {
        return;
    }

    objectHash.remove(hostname);
    socketHash.remove(hostname);
}

void NetworkManager::Private::updateSocket()
{
    if (!socket.isNull())
    {
        delete socket.data();
        socket.clear();
    }

    if (!keepaliveHost.isValid())
    {
        return;
    }

    localPort = keepaliveHost.port;

    socket = qobject_cast<QUdpSocket *>(q->createConnection(NetworkManager::UdpSocket));
    socket->setParent(this);
    connect(socket.data(), SIGNAL(readyRead()), this, SLOT(onDatagramReady()));

    if (!socket->bind(keepaliveHost.port))
    {
        LOG_ERROR(QString("Unable to bind port %1: %2").arg(keepaliveHost.port).arg(socket->errorString()));
    }
}

void NetworkManager::Private::updateTimer()
{
    TimingPtr timing = settings->config()->keepaliveTiming();

    if (!timing)
    {
        return;
    }

    QSharedPointer<PeriodicTiming> periodicTiming = timing.dynamicCast<PeriodicTiming>();
    Q_ASSERT(periodicTiming);

    int interval = periodicTiming->interval();

    if (interval < 1000)
    {
        LOG_ERROR("Keepalive interval < 1 sec will not be accepted.");
        return;
    }
    else
    {
        if (timer.interval() != interval)
        {
            LOG_DEBUG(QString("Keepalive set to %1 sec.").arg(interval / 1000));
        }
    }

    timer.setInterval(interval);
    timer.start();
}

class NetworkManagerMeasurementObserver : public MeasurementObserver
{
public:
    NetworkManager *networkManager;
    NetworkManager::SocketType socketType;
    quint16 localPort;

    QHostAddress host;
    quint16 port;

    QUuid measurementUuid;

    // MeasurementObserver interface
    void created(const MeasurementPtr &measurement)
    {
        QAbstractSocket *socket = networkManager->createConnection(socketType);

        QUdpSocket *udpSocket = qobject_cast<QUdpSocket *>(socket);

        if (!udpSocket->bind(localPort))
        {
            LOG_ERROR("Cannot bind socket");
            return;
        }

        // ACK the connection
        udpSocket->writeDatagram(QByteArray(), host, port);
        measurement->setPeerSocket(udpSocket);
        measurement->setMeasurementUuid(measurementUuid);
    }
};

void NetworkManager::Private::processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
    QString hostAndPort = QString("%1:%2").arg(host.toString()).arg(port);

    if (datagram.isEmpty())
    {
        LOG_DEBUG(QString("Received empty datagram from %1").arg(hostAndPort));
        return;
    }

    LOG_DEBUG(QString("Received datagram from %1: %2").arg(hostAndPort).arg(QString::fromUtf8(datagram)));
    //    if (settings->config()->keepaliveAddress() == hostAndPort) {
    // Master server
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(datagram, &error);

    if (error.error == QJsonParseError::NoError)
    {
        QJsonObject root = document.object();

        QString replyError = root.value("error").toString();

        if (!replyError.isEmpty())
        {
            LOG_ERROR(QString("Error from server: %1").arg(root.value("error").toString()));
            return;
        }

        PeerRequest request = PeerRequest::fromVariant(document.toVariant());

        if (handledMeasureUuids.contains(request.measurementUuid))
        {
            LOG_DEBUG("Measurement uuid is already handled, skipping second try");
            return;
        }

        handledMeasureUuids.insert(request.measurementUuid);

        MeasurementObserver *observer = NULL;

        if (request.protocol == NetworkManager::UdpSocket)
        {
            NetworkManagerMeasurementObserver *tempObs = new NetworkManagerMeasurementObserver;
            tempObs->networkManager = q;
            tempObs->socketType = request.protocol;
            tempObs->localPort = 5106; // FIXME: Don't hardcode this here
            tempObs->host = host; //request.peer;
            tempObs->port = port;
            tempObs->measurementUuid = request.measurementUuid;

            observer = tempObs;
        }

        // FIXME: We can't assign the Peer socket to Measurement since this is created in a separate thread
        //        and we can't access.

        TimingPtr timing(new ImmediateTiming);
        TestDefinition testDefinition(request.taskId, request.measurement, timing,
                                      request.measurementDefinition, Precondition());

        // Bypass scheduler and run directly on the executor
        scheduler->executor()->execute(testDefinition, observer);
    }
    else
    {
        LOG_ERROR(QString("Invalid JSon: %1").arg(error.errorString()));
    }

    //    } else {
    // TODO: Process incoming data
    //    }
}

void NetworkManager::Private::responseChanged()
{
    // Update the keepalive host address
    keepaliveHost = NetworkHelper::remoteHost(settings->config()->keepaliveAddress());

    updateSocket();
    //updateTimer(); TODO: deactivated for now

    // Lookup the host
    keepaliveAddressLookup.setName(keepaliveHost.host);
    keepaliveAddressLookup.lookup();
}

void NetworkManager::Private::timeout()
{
    if (keepaliveAddress.isNull())
    {
        LOG_WARNING("Invalid keepalive address (normal at first app start)");
        return;
    }

    QString sessionId = settings->apiKey();

    if (sessionId.isEmpty())
    {
        LOG_WARNING("Empty session id");
        return;
    }

    QStringList srcIp;
    srcIp.append(NetworkHelper::localIpAddress().toString());

    QVariantMap map;
    map.insert("type", "keepalive");
    map.insert("session_id", sessionId);
    map.insert("src_ip", srcIp);
    map.insert("src_port", localPort);

    QByteArray data = QJsonDocument::fromVariant(map).toJson();
    socket->writeDatagram(data, keepaliveAddress, keepaliveHost.port);

    LOG_DEBUG("Alive packet sent");
}

void NetworkManager::Private::onDatagramReady()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        QHostAddress host;
        quint16 port;
        socket->readDatagram(datagram.data(), datagram.size(), &host, &port);

        // Process the datagram
        processDatagram(datagram, host, port);
    }
}

void NetworkManager::Private::lookupFinished()
{
    return; // TODO disabled for now
    // Check for errors
    if (keepaliveAddressLookup.error() != QDnsLookup::NoError)
    {
        LOG_ERROR(QString("Unable to look up host %1: %2").arg(keepaliveAddressLookup.name()).arg(keepaliveAddressLookup.errorString()));
        return;
    }

    foreach(const QDnsHostAddressRecord& record, keepaliveAddressLookup.hostAddressRecords())
    {
        // TODO: Take multiple host records
        QHostAddress value = record.value();
        if (!value.isNull())
        {
            keepaliveAddress = value;
            timeout();
            break;
        }
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

bool NetworkManager::init(Scheduler *scheduler, Settings *settings)
{
    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(responseChanged()));

    d->scheduler = scheduler;
    d->settings = settings;
    d->responseChanged();

    emit d->ncm.updateConfigurations();

    return true;
}

void NetworkManager::setRunning(bool running)
{
    if (isRunning() == running)
    {
        return;
    }

    if (!running)
    {
        d->timer.stop();
    }
    else
    {
        d->updateTimer();
    }
}

bool NetworkManager::isRunning() const
{
    return d->timer.isActive();
}

bool NetworkManager::onMobileConnection() const
{
#if defined(Q_OS_ANDROID)
     return d->networkInfo.callMethod<jboolean>("isOnMobileConnection");
#else
    QNetworkInfo::NetworkMode mode = connectionMode();

    if (mode != QNetworkInfo::WlanMode && mode != QNetworkInfo::EthernetMode)
    {
        LOG_DEBUG(QString("Connection type: %1").arg(mode));
        return true;
    }

    return false;
#endif
}

QNetworkInfo::NetworkMode NetworkManager::connectionMode() const
{
    QNetworkInfo::NetworkMode mode;

#if defined(Q_OS_ANDROID)
    mode = static_cast<QNetworkInfo::NetworkMode>(d->networkInfo.callMethod<jint>("connectionMode"));
#elif defined(Q_OS_IOS)
    mode = QNetworkInfo::WlanMode;
#else
    mode = d->networkInfo.currentNetworkMode();
    // QNetworkInfo does not check for the linux "Predictable Network Interface Names".
    // The following code fixes this.
    if (mode == QNetworkInfo::UnknownMode)
    {
        QList<QNetworkConfiguration> confList = d->ncm.allConfigurations(QNetworkConfiguration::Active);

        foreach (const QNetworkConfiguration &conf, confList)
        {
            QString name = conf.name();

            if (name.contains("enp"))
            {
                return QNetworkInfo::EthernetMode;
            }

            if (name.contains("wlp"))
            {
                return QNetworkInfo::WlanMode;
            }
        }
    }
#endif

    return mode;
}

QAbstractSocket *NetworkManager::connection(const QString &hostname, NetworkManager::SocketType socketType) const
{
    Q_UNUSED(socketType);
    return d->socketHash.value(hostname);
}

QAbstractSocket *NetworkManager::createConnection(NetworkManager::SocketType socketType)
{
    QAbstractSocket *socket = d->createSocket(socketType);

    if (!socket)
    {
        LOG_ERROR("Unknown socket type requested");
        return NULL;
    }

    //d->socketHash.insert(hostname, socket);
    //d->objectHash.insert(hostname, socket);

    return socket;
}

QAbstractSocket *NetworkManager::establishConnection(const QString &hostname,
                                                     const TaskId &taskId,
                                                     const QString &measurement,
                                                     MeasurementDefinitionPtr measurementDefinition,
                                                     NetworkManager::SocketType socketType)
{
    QAbstractSocket *socket = d->createSocket(socketType);

    if (!socket)
    {
        LOG_ERROR("Unknown socket type requested");
        return NULL;
    }

    RemoteHost remote = NetworkHelper::remoteHost(hostname);
    RemoteHost aliveRemote = d->keepaliveHost;

    if (false && d->keepaliveAddress.isNull()) // TODO deactivated for now
    {
        LOG_ERROR(QString("Invalid keepalivealive address: '%1' can't talk to alive server").arg(
                      d->keepaliveAddress.toString()));
        delete socket;
        return NULL;
    }

    measurementDefinition->measurementUuid = QUuid::createUuid();

    PeerRequest request;
    request.measurement = measurement;
    request.taskId = taskId;
    request.measurementDefinition = measurementDefinition->toVariant();
    request.measurementUuid = measurementDefinition->measurementUuid;
    request.peer = remote.host;
    request.port = aliveRemote.port;
    request.protocol = socketType;

    // If for some reason our packet gets routed back to us, don't handle it
    d->handledMeasureUuids.insert(measurementDefinition->measurementUuid);

    QUdpSocket *testSocket = d->socket.data();

    if (socketType != TcpSocket)
    {
        if (!socket->bind(remote.port))
        {
            LOG_ERROR(QString("Unable to bind source port to %1").arg(remote.port));
            delete socket;
            return NULL;
        }

        QUdpSocket *udpSocket = qobject_cast<QUdpSocket *>(socket);
        udpSocket->writeDatagram(QByteArray(), QHostAddress(remote.host), remote.port);

        testSocket = udpSocket;
    }

    QByteArray data = QJsonDocument::fromVariant(request.toVariant()).toJson();

    // Step one: Send test offer to peer directly
    testSocket->writeDatagram(data, QHostAddress(remote.host), d->localPort);

    // Step two: Send test offer to peer via alive-server
    //testSocket->writeDatagram(data, d->keepaliveAddress, aliveRemote.port); // TODO deactivated for now

    LOG_TRACE("Sent test offer to peer and alive-server");

    if (socketType != UdpSocket)
    {
        // Final step: Connect to remote host
        //if (socket->waitForReadyRead(5000))
        //    return socket;

        const int tries = 20;

        for (int i = 0; i < tries; ++i)
        {
            socket->connectToHost(remote.host, remote.port);

            if (socket->waitForConnected(5000 / tries))
            {
                return socket;
            }
        }

        LOG_ERROR(QString("Unable to connect tcp socket in %3 tries to %1: %2").arg(hostname).arg(socket->errorString()).arg(
                      tries));
    }
    else
    {
        if (!testSocket->waitForReadyRead(5000))
        {
            LOG_ERROR("Remote did not answer for 5 sec, aborting connection.");
        }
        else
        {
            // TODO: Read the first (empty) datagram
            QHostAddress packetHost;
            testSocket->readDatagram(0, 0, &packetHost);

            if (packetHost != QHostAddress(remote.host))
            {
                LOG_ERROR("Received connection packet from wrong host!");
            }
            else
            {
                return socket;
            }
        }
    }

    delete socket;
    return NULL;
}

QTcpServer *NetworkManager::createServerSocket()
{
    // TODO: Connect to traffic counter
    QTcpServer *server = new QTcpServer;
    return server;
}

bool NetworkManager::allInterfacesDown() const
{
    QNetworkInterface::InterfaceFlags flags;

    foreach (const QNetworkInterface &iface, QNetworkInterface::allInterfaces())
    {
        flags = iface.flags();

        if (flags & QNetworkInterface::IsLoopBack)
        {
            continue;
        }

        if (flags & (QNetworkInterface::IsUp | QNetworkInterface::IsRunning))
        {
            return false;
        }
    }

    return true;
}

#include "networkmanager.moc"
