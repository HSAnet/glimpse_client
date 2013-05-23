#ifndef PACKETTRAIN_H
#define PACKETTRAIN_H

#include "test.h"

#include <QObject>
#include <QStringList>
#include <QTimer>

class PacketTrain : public AbstractTest
{
    Q_OBJECT
    Q_INTERFACES(AbstractTest)
    Q_PROPERTY(int packetCount READ packetCount NOTIFY packetCountChanged)

public:
    explicit PacketTrain(QObject *parent = 0);
    ~PacketTrain();

    int packetCount() const { return packetCounter; }

    // AbstractTest overrides
    QString name() const;
    bool isMaster() const;

    bool initialize(const PeerList &peers, bool master, QUdpSocket* socket);
    void uninitialize();

    bool start();
    bool stop();
    bool isFinished() const;

    void processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port);

    QVariant data(int role) const;

    QVariant result() const;

signals:
    void packetCountChanged(int packetCount);

private slots:
    void timeout();

protected:
    bool isInitialized;
    PeerList peers;
    QVariantList respondingPeers;
    bool master;
    QUdpSocket* socket;

    int packetCounter;
    QTimer timer;
    QTimer timeouter;
    bool running;
};

#endif // PACKETTRAIN_H
