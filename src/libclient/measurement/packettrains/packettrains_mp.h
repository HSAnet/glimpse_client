#ifndef PACKETTRAINS_MP_H
#define PACKETTRAINS_MP_H

#include <QTimer>
#include <QUdpSocket>

#include "../measurement.h"
#include "packettrainsdefinition.h"

class PacketTrainsMP : public Measurement
{
    Q_OBJECT
public:
    explicit PacketTrainsMP();
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    PacketTrainsDefinitionPtr definition;
    QUdpSocket* m_udpSocket;
    QTimer m_timer;
    QList<QList<struct msrmnt> > m_rec;
    quint16 m_packetsReceived;
    QList<int> m_sendSpeed;
    QList<int> m_recvSpeed;

public slots:
    void readPendingDatagrams();
    void eval();
    void handleError(QAbstractSocket::SocketError socketError);
};

#endif // PACKETTRAINS_MP_H
