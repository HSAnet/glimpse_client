#include "packettrain.h"

#include <QDebug>

PacketTrain::PacketTrain(QObject *parent)
: QObject(parent)
, isInitialized(false)
, packetCounter(0)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));

    timer.setInterval(100);
}

PacketTrain::~PacketTrain()
{
}

bool PacketTrain::initialize(const PeerList &peers, bool master, QUdpSocket *socket)
{
    if ( isInitialized || peers.isEmpty() )
        return false;

    if ( peers.isEmpty() ) {
        qDebug() << "Peer list is empty, can't initialize packettrain test";
        return false;
    }

    this->peers = peers;
    this->master = master;
    this->socket = socket;

    return (isInitialized=true);
}

void PacketTrain::uninitialize()
{
}

bool PacketTrain::start()
{
    qDebug() << Q_FUNC_INFO;

    // Master waits for incoming packets
    if ( master ) {
        return true;
    }

    // Clients start sending data now
    timer.start();

    return true;
}

bool PacketTrain::stop()
{
    qDebug() << Q_FUNC_INFO;

    timer.stop();
    return true;
}

bool PacketTrain::isFinished() const
{
    return timer.isActive() || packetCounter > 100;
}

void PacketTrain::processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
    if ( master )
        ++packetCounter;

    qDebug() << Q_FUNC_INFO << host.toString() << port;
}

void PacketTrain::timeout()
{
    if ( packetCounter++ > 100 ) {
        stop();
        return;
    }

    foreach(const Peer& peer, peers) {
        QByteArray data;
        data.resize(2500);
        data.fill('X');

        socket->writeDatagram(data, peer.host, peer.port);
    }
}
