#ifndef BTC_MP_H
#define BTC_MP_H

#include "../measurement.h"
#include "btc_definition.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class BulkTransportCapacityMP : public Measurement
{
    Q_OBJECT

public:
    explicit BulkTransportCapacityMP(QObject *parent = 0);

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    qint64 sendResponse(quint64 bytes);

    BulkTransportCapacityDefinitionPtr definition;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;

private slots:
    void resetServer();
    void newClientConnection();
    void receiveRequest();
    void handleError(QAbstractSocket::SocketError socketError);
};

#endif // BTC_MP_H
