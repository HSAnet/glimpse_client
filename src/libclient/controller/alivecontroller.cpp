#include "alivecontroller.h"
#include "../settings.h"
#include "../networkhelper.h"
#include "../network/networkmanager.h"
#include "../log/logger.h"

#include <QPointer>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QStringList>
#include <QTimer>
#include <QDebug>

LOGGER(AliveController)

class AliveController::Private : public QObject
{
    Q_OBJECT

public:
    Private()
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

    // Properties
    QTimer timer;
    QPointer<QUdpSocket> socket;
    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    quint16 localPort;

    // Functions
    void updateSocket();
    void updateTimer();
    void processDatagram(const QByteArray& datagram, const QHostAddress& host, quint16 port);

public slots:
    void responseChanged();
    void timeout();
    void onDatagramReady();
};

void AliveController::Private::updateSocket()
{
    if (!socket.isNull())
        socket->deleteLater();

    QString keepaliveAddress = settings->config()->keepaliveAddress();
    RemoteHost remote = NetworkHelper::remoteHost(keepaliveAddress);
    if (!remote.isValid())
        return;

    localPort = remote.port;

    socket = qobject_cast<QUdpSocket*>( networkManager->createConnection(keepaliveAddress, NetworkManager::UdpSocket) );
    connect(socket.data(), SIGNAL(readyRead()), this, SLOT(onDatagramReady()));
    if (!socket->bind(remote.port)) {
        LOG_INFO(QString("Unable to bind port %1: %2").arg(remote.port).arg(socket->errorString()));
    }
}

void AliveController::Private::updateTimer()
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

void AliveController::Private::processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
    QString hostAndPort = QString("%1:%2").arg(host.toString()).arg(port);
    if (settings->config()->keepaliveAddress() == hostAndPort) {
        // Master server
    } else {
        // Someone else
    }
}

void AliveController::Private::responseChanged()
{
    updateSocket();
    updateTimer();

    // Send the first timeout now
    timeout();
}

void AliveController::Private::timeout()
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

void AliveController::Private::onDatagramReady()
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

AliveController::AliveController(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

AliveController::~AliveController()
{
    delete d;
}

bool AliveController::init(NetworkManager *networkManager, Settings *settings)
{
    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(responseChanged()));

    d->settings = settings;
    d->networkManager = networkManager;
    d->responseChanged();

    return true;
}

void AliveController::setRunning(bool running)
{
    if (isRunning() == running)
        return;

    if ( !running )
        d->timer.stop();
    else
        d->updateTimer();
}

bool AliveController::isRunning() const
{
    return d->timer.isActive();
}

#include "alivecontroller.moc"
