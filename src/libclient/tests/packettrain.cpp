#include "packettrain.h"

#include <QDebug>

PacketTrain::PacketTrain(QObject *parent)
: AbstractTest(parent)
, isInitialized(false)
, packetCounter(0)
, running(false)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(&timeouter, SIGNAL(timeout()), this, SLOT(stop()));

    timer.setInterval(10);

    timeouter.setInterval(5000);
    timeouter.setSingleShot(true);
}

PacketTrain::~PacketTrain()
{
}

QString PacketTrain::name() const
{
    return "packettrain";
}

bool PacketTrain::isMaster() const
{
    return master;
}

bool PacketTrain::initialize(const PeerList &peers, bool master, QUdpSocket *socket)
{
    if ( isInitialized )
    {
        return false;
    }

    if ( peers.isEmpty() )
    {
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

    running = true;

    timeouter.start();

    // Master waits for incoming packets
    if ( master )
    {
        return true;
    }

    // Clients start sending data now
    timer.start();

    return true;
}

bool PacketTrain::stop()
{
    qDebug() << Q_FUNC_INFO;

    running = false;

    timer.stop();
    return true;
}

bool PacketTrain::isFinished() const
{
    if ( master )
    {
        return packetCounter >= 100;
    }
    else
    {
        return !timer.isActive();
    }
}

void PacketTrain::processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
    QString peer = Peer(host, port).toString();
    if ( !respondingPeers.contains(peer) )
    {
        respondingPeers.append(peer);
    }

    if ( master )
    {
        emit packetCountChanged(packetCounter++);
    }

    timeouter.start();

    //qDebug() << Q_FUNC_INFO << host.toString() << port << packetCounter << "asdfff";
}

QVariant PacketTrain::data(int role) const
{
    Q_UNUSED(role)
    return QVariant();
}

QVariant PacketTrain::result() const
{
    QVariantMap values;
    values.insert("master", master);
    values.insert(master ? "received-packets" : "sent-packets", packetCounter);
    values.insert("responding-peers", respondingPeers);
    return values;
}

void PacketTrain::timeout()
{
    emit packetCountChanged(packetCounter);
    if ( packetCounter++ >= 100 )
    {
        stop();
        return;
    }

    QByteArray data;
    data.resize(512);
    data.fill('X');

    foreach(const Peer& peer, peers)
    {
        socket->writeDatagram(data, peer.host, peer.port);
    }
}
