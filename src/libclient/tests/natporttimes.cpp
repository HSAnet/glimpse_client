#include "natporttimes.h"

NatPortTimes::NatPortTimes(QObject *parent)
: isInitialized(false)
, gotFirstPacket(false)
{
    connect(&puncher, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

NatPortTimes::~NatPortTimes()
{
}

QString NatPortTimes::name() const
{
    return "natporttimes";
}

bool NatPortTimes::isMaster() const
{
    return master;
}

bool NatPortTimes::initialize(const PeerList &peers, bool master, QUdpSocket *socket)
{
    if ( isInitialized )
        return true;

    if ( !puncher.bind(32010) )
        return false;

    this->master = master;
    this->peers = peers;
    this->socket = socket;

    return (isInitialized=true);
}

void NatPortTimes::uninitialize()
{
}

bool NatPortTimes::start()
{
    foreach(const Peer& peer, peers)
        puncher.writeDatagram(QByteArray(), peer.host, peer.port);

    return true;
}

bool NatPortTimes::stop()
{
    return true;
}

bool NatPortTimes::isFinished() const
{
    return true;
}

void NatPortTimes::processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
}

QVariant NatPortTimes::data(int role) const
{
    return QVariant();
}

void NatPortTimes::readyRead()
{
    while ( puncher.hasPendingDatagrams() ) {
        QByteArray datagram;
        datagram.resize(puncher.pendingDatagramSize());

        QHostAddress host;
        quint16 port;

        puncher.readDatagram(datagram.data(), datagram.size(), &host, &port);

        // Throw away the first datagram
        if (!gotFirstPacket) {
            gotFirstPacket = true;
            continue;
        }

        // TODO: Code
    }
}
