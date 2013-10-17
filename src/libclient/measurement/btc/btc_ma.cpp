#include "btc_ma.h"
#include "../../log/logger.h"

#include <QDataStream>

LOGGER(BulkTransportCapacityMA)

BulkTransportCapacityMA::BulkTransportCapacityMA(QObject *parent)
: Measurement(parent)
{
}

bool BulkTransportCapacityMA::start()
{
    LOG_INFO(QString("Connect to %1:%2").arg(definition->host).arg(definition->port));
    m_tcpSocket->connectToHost(definition->host, definition->port);
    return true;
}

void BulkTransportCapacityMA::sendRequest(quint64 bytes)
{
    m_bytesExpected = bytes;
    QDataStream out(m_tcpSocket);
    out<<bytes;
}

void BulkTransportCapacityMA::sendInitialRequest()
{
    LOG_INFO("Sending initial data size to server");
    sendRequest(definition->initialDataSize);
}

void BulkTransportCapacityMA::receiveResponse()
{
    int time = m_time.elapsed();

    if(m_time.isNull())
    {
        m_bytesReceived = 0;
        m_totalBytesReceived = m_tcpSocket->bytesAvailable();
        m_lasttime = 0;
        m_tcpSocket->readAll(); // ignor the first data received
        m_time.restart();
    }
    else
    {
        qint64 bytes = m_tcpSocket->bytesAvailable();
        m_bytesReceived += bytes;
        m_totalBytesReceived += bytes;
        m_tcpSocket->readAll();
        m_lasttime = time;
    }

    if(m_totalBytesReceived >= m_bytesExpected)
    {
        if (m_preTest)
        {
            qreal downloadSpeed = ((qreal)m_bytesReceived / 1024) / ((qreal)m_lasttime / 1000); // kbyte/s

            LOG_INFO(QString("Speed: %1 KByte/s").arg(downloadSpeed, 0, 'g', 0));

            // TODO this is were the magic happens
            m_bytesExpected = 100*1024;

            // set timer to zero
            m_time = QTime();
            m_preTest = false;

            LOG_INFO("Sending test data size to server");

            // set next state
            sendRequest(m_bytesExpected);
        }
        else
        {
            qreal downloadSpeed = ((qreal)m_bytesReceived / 1024) / ((qreal)m_lasttime / 1000); // kbyte/s
            LOG_INFO(QString("Speed: %1 KByte/s").arg(downloadSpeed, 0, 'g', 0));
        }
    }
}

void BulkTransportCapacityMA::serverDisconnected()
{
    LOG_WARNING("Server closed connection, this should not happen");
}

void BulkTransportCapacityMA::handleError(QAbstractSocket::SocketError socketError)
{
    if(socketError == QAbstractSocket::RemoteHostClosedError)
        return;

    QAbstractSocket* socket = qobject_cast<QAbstractSocket*>(sender());
    LOG_ERROR(QString("Socket Error: %1").arg(socket->errorString()));
}

Measurement::Status BulkTransportCapacityMA::status() const
{
    return Unknown;
}

bool BulkTransportCapacityMA::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    definition = measurementDefinition.dynamicCast<BulkTransportCapacityDefinition>();
    if ( definition.isNull() ) {
        LOG_WARNING("Definition is empty");
    }

    m_tcpSocket = qobject_cast<QTcpSocket*>(networkManager->createConnection(NetworkManager::TcpSocket));
    m_tcpSocket->setParent(this);
    m_bytesExpected = 0;
    m_preTest = true;

    // Signal for new data
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveResponse()));

    // Signal for errors
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));

    // Signal for end of data transmission
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));

    // Signal to start measurement when the client is connected
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(sendInitialRequest()));

    return true;
}

bool BulkTransportCapacityMA::stop()
{
    return true;
}

ResultPtr BulkTransportCapacityMA::result() const
{
    return ResultPtr();
}
