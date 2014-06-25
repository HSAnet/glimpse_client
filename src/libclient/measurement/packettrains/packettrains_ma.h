#ifndef PACKETTRAINS_MA_H
#define PACKETTRAINS_MA_H

#include <QUdpSocket>

#include "../measurement.h"
#include "packettrainsdefinition.h"

class PacketTrainsMA : public Measurement
{
    Q_OBJECT
public:
    explicit PacketTrainsMA();
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

private:
    PacketTrainsDefinitionPtr definition;
    QUdpSocket *m_udpSocket;

public slots:
    void handleError(QAbstractSocket::SocketError socketError);
};

#endif // PACKETTRAINS_MA_H
