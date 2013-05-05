#ifndef NATPORTTIMES_H
#define NATPORTTIMES_H

#include "test.h"

#include <QUdpSocket>

class NatPortTimes : public AbstractTest
{
    Q_OBJECT

public:
    explicit NatPortTimes(QObject *parent = 0);
    ~NatPortTimes();

    // AbstractTest overrides
    QString name() const;
    bool isMaster() const;

    bool initialize(const PeerList &peers, bool master, QUdpSocket *socket);
    void uninitialize();

    bool start();
    bool stop();
    bool isFinished() const;

    void processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port);

    QVariant data(int role) const;

private slots:
    void readyRead();

protected:
    bool isInitialized;
    bool master;
    bool gotFirstPacket;

    QUdpSocket* socket;
    QUdpSocket puncher;
    PeerList peers;
};

#endif // NATPORTTIMES_H
