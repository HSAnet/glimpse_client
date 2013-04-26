#ifndef PACKETTRAIN_H
#define PACKETTRAIN_H

#include "test.h"

#include <QObject>
#include <QStringList>
#include <QTimer>

class PacketTrain : public QObject
                  , public AbstractTest
{
    Q_OBJECT
    Q_INTERFACES(AbstractTest)

public:
    explicit PacketTrain(QObject *parent = 0);
    ~PacketTrain();

    // AbstractTest overrides
    bool initialize(const PeerList &peers, bool master, QUdpSocket* socket);
    void uninitialize();

    bool start();
    bool stop();
    bool isFinished() const;

    void processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port);

private slots:
    void timeout();

protected:
    bool isInitialized;
    PeerList peers;
    bool master;
    QUdpSocket* socket;

    int packetCounter;
    QTimer timer;
};

#endif // PACKETTRAIN_H
