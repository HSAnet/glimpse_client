#ifndef BTC_MA_H
#define BTC_MA_H

#include "../measurement.h"
#include "btc_definition.h"

#include <QObject>
#include <QTcpSocket>
#include <QTime>

class BulkTransportCapacityMA : public QObject
                              , public Measurement
{
    Q_OBJECT

public:
    explicit BulkTransportCapacityMA(QObject *parent = 0);

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    void sendRequest(quint64 bytes);

    BulkTransportCapacityDefinitionPtr definition;
    bool m_preTest;
    QTcpSocket *m_tcpSocket;
    QTime m_time;
    quint64 m_bytesReceived; // without first packets
    quint64 m_totalBytesReceived; // with first packets
    quint64 m_bytesExpected;
    int m_lasttime;

private slots:
    void sendInitialRequest();
    void receiveResponse();
    void serverDisconnected();
    void handleError(QAbstractSocket::SocketError socketError);
};

#endif // BTC_MA_H
