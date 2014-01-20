#include "btc_ma.h"
#include "../../log/logger.h"
#include "../../network/networkmanager.h"

#include <QDataStream>

LOGGER(BulkTransportCapacityMA);

BulkTransportCapacityMA::BulkTransportCapacityMA(QObject *parent)
: Measurement(parent)
, m_preTest(true)
, m_tcpSocket(NULL)
, m_downloadSpeed(0.0)
, m_lasttime(-1)
, m_status(Unknown)
{
}

bool BulkTransportCapacityMA::start()
{
    m_status = BulkTransportCapacityMA::Running;
    sendInitialRequest();
    return true;
}

void BulkTransportCapacityMA::sendRequest(quint64 bytes)
{
    // invalidate timer
    m_time.invalidate();

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
    qint64 time = m_time.nsecsElapsed();

    if (!m_time.isValid())
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
        qreal downloadSpeed = (bytes / 1024.0) / ((((time - m_lasttime) / 1000.0) / 1000) / 1000); // kbyte/s
        LOG_INFO(QString("Tmp Speed: %1 KByte/s").arg(downloadSpeed, 0, 'f', 0).arg(time).arg(m_lasttime).arg(bytes));

        m_bytesReceived += bytes;
        m_totalBytesReceived += bytes;
        m_tcpSocket->readAll();
        m_lasttime = time;
    }

    if (m_totalBytesReceived >= m_bytesExpected)
    {
        if (m_preTest)
        {
            qreal downloadSpeed = (m_bytesReceived / 1024.0) / (((m_lasttime / 1000.0) / 1000) / 1000); // kbyte/s

            LOG_INFO(QString("Speed: %1 KByte/s").arg(downloadSpeed, 0, 'f', 0));

            // TODO this is were the magic happens
            m_bytesExpected = 1024*1024*2; // 5 MB

            m_preTest = false;

            LOG_INFO("Sending test data size to server");

            // set next state
            sendRequest(m_bytesExpected);
        }
        else
        {
            m_downloadSpeed = (m_bytesReceived / 1024.0) / (((m_lasttime / 1000.0) / 1000) / 1000); // kbyte/s
            LOG_INFO(QString("Speed: %1 KByte/s").arg(m_downloadSpeed, 0, 'f', 0));

            m_status = BulkTransportCapacityMA::Finished;

            emit finished();
        }
    }
}

void BulkTransportCapacityMA::serverDisconnected()
{
    if (m_status != BulkTransportCapacityMA::Finished)
    {
        LOG_WARNING("Server closed connection, this should not happen");
    }
}

void BulkTransportCapacityMA::handleError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QAbstractSocket::RemoteHostClosedError)
    {
        return;
    }

    QAbstractSocket* socket = qobject_cast<QAbstractSocket*>(sender());
    LOG_ERROR(QString("Socket Error: %1").arg(socket->errorString()));
}

Measurement::Status BulkTransportCapacityMA::status() const
{
    return m_status;
}

bool BulkTransportCapacityMA::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    definition = measurementDefinition.dynamicCast<BulkTransportCapacityDefinition>();
    if ( definition.isNull() )
    {
        LOG_WARNING("Definition is empty");
    }

    QString hostname = QString("%1:%2").arg(definition->host).arg(definition->port);

    m_tcpSocket = qobject_cast<QTcpSocket*>(networkManager->establishConnection(hostname, "btc_mp", definition->toVariant(), NetworkManager::TcpSocket));
    if (!m_tcpSocket)
    {
        LOG_ERROR("Preparation failed");
        return false;
    }

    m_tcpSocket->setParent(this);
    m_bytesExpected = 0;
    m_preTest = true;

    // Signal for new data
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveResponse()));

    // Signal for errors
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));

    // Signal for end of data transmission
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));

    return true;
}

bool BulkTransportCapacityMA::stop()
{
    if (m_tcpSocket)
    {
        m_tcpSocket->disconnectFromHost();
        if (m_tcpSocket->state() != QTcpSocket::UnconnectedState &&
                !m_tcpSocket->waitForDisconnected(1000))
        {
            return false;
        }
    }

    return true;
}

ResultPtr BulkTransportCapacityMA::result() const
{
    QVariantMap map;
    map.insert("download_speed", m_downloadSpeed);

    return ResultPtr(new Result(QDateTime::currentDateTime(), map, QVariant()));
}
