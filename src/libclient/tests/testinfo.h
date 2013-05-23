#ifndef TESTINFO_H
#define TESTINFO_H

#include <QHostAddress>

struct Peer
{
    Peer(const QHostAddress& host, quint16 port)
    : host(host)
    , port(port)
    {
    }

    QString toString() {
        return QString("%1:%2").arg(host.toString()).arg(port);
    }

    QHostAddress host;
    quint16 port;
};

typedef QList<Peer> PeerList;

class TestInfo
{
public:
    TestInfo(const QString& name, const PeerList& peers, bool isMaster)
    : name(name)
    , peers(peers)
    , isMaster(isMaster)
    {
    }

    //QUuid id;
    QString name;
    PeerList peers;
    bool isMaster;
};

#endif // TESTINFO_H
