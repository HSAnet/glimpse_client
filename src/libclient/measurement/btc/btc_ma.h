#ifndef BTC_MA_H
#define BTC_MA_H

#include "../measurement.h"
#include "btc_definition.h"

#include <QObject>
#include <QTcpSocket>
#include <QElapsedTimer>

class BulkTransportCapacityMA : public Measurement
{
    Q_OBJECT

public:
    explicit BulkTransportCapacityMA(QObject *parent = 0);

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

private:
    void sendRequest(quint64 bytes);
    void calculateResult();

    BulkTransportCapacityDefinitionPtr definition;
    bool m_preTest;
    QTcpSocket *m_tcpSocket;
    QElapsedTimer m_time;
    qint64 m_bytesReceived; // without first packets
    qint64 m_totalBytesReceived; // with first packets
    qint64 m_bytesExpected;
    qint64 m_lasttime;
    QList<qreal> m_downloadSpeeds;
    Status m_status;
    QVector<qint64> m_bytesReceivedList;
    QVector<qint64> m_times;

private slots:
    void receiveResponse();
    void serverDisconnected();
    void handleError(QAbstractSocket::SocketError socketError);
};

#endif // BTC_MA_H
