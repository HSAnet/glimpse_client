#include "controlcontroller.h"
#include "../network/networkmanager.h"
#include "../scheduler/scheduler.h"
#include "../settings.h"
#include "alivecontroller.h"

#include <QPointer>
#include <QUdpSocket>

class ControlController::Private : public QObject
{
    Q_OBJECT

public:
    Private(ControlController* q)
    : q(q)
    , socket(NULL)
    {
    }

    ControlController* q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Scheduler> scheduler;
    QPointer<Settings> settings;

    AliveController aliveController;

    QUdpSocket* socket;

    // Functions
    void processDatagram(const QByteArray& datagram, const QHostAddress& host, quint16 port);

public slots:
    void onDatagramReady();
};

void ControlController::Private::processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
    QString hostAndPort = QString("%1:%2").arg(host.toString()).arg(port);
    if (settings->config()->keepaliveAddress() == hostAndPort) {
        // Master server
    } else {
        // Someone else
    }
}

void ControlController::Private::onDatagramReady()
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

ControlController::ControlController(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

ControlController::~ControlController()
{
    delete d;
}

bool ControlController::init(NetworkManager *networkManager, Scheduler *scheduler, Settings *settings)
{
    d->scheduler = scheduler;
    d->settings = settings;
    d->networkManager = networkManager;
    d->socket = qobject_cast<QUdpSocket*>(networkManager->createConnection("mplane0.informatik.hs-augsburg:5543", NetworkManager::UdpSocket));
    d->socket->setParent(d);
    // FIXME: Remove the hardcoded port
    d->socket->bind(1337);

    // Also initialize the alive controller
    d->aliveController.setSocket(d->socket);
    d->aliveController.init(settings);

    return true;
}

#include "controlcontroller.moc"
