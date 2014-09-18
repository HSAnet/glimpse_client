#include "btc_mp.h"
#include "../../log/logger.h"
#include "../../network/networkmanager.h"

#include <QDataStream>

LOGGER(BulkTransportCapacityMP);

BulkTransportCapacityMP::BulkTransportCapacityMP(QObject *parent)
: Measurement(parent)
, m_tcpServer(NULL)
{
    connect(this, SIGNAL(error(const QString &)), this,
            SLOT(setErrorString(const QString &)));
    resetServer();
}

bool BulkTransportCapacityMP::start()
{
    // Start listening
    bool ret = m_tcpServer->listen(QHostAddress::Any, definition->port);
    LOG_DEBUG(QString("Listening on port %1: %2").arg(definition->port).arg(ret));
    return ret;
}

qint64 BulkTransportCapacityMP::sendResponse(quint64 bytes)
{
    // send data back
    QByteArray data;
    data.resize(bytes);
    data.fill('X'); // TODO: do we need to randomize this to prevent compression on the path?
    return m_tcpSocket->write(data);
}

void BulkTransportCapacityMP::resetServer()
{
    m_tcpSocket = 0;
}

void BulkTransportCapacityMP::newClientConnection()
{
    LOG_INFO("New client connection");

    if (!m_tcpSocket)
    {
        m_tcpSocket = m_tcpServer->nextPendingConnection();

        connect(m_tcpSocket, SIGNAL(disconnected()), this, SIGNAL(finished()));
        connect(m_tcpSocket, SIGNAL(disconnected()), m_tcpSocket, SLOT(deleteLater()));
        connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveRequest()));
        connect(m_tcpSocket, SIGNAL(destroyed()), this, SLOT(resetServer()));
    }
    else
    {
        LOG_WARNING("There is already a client connected, abort");
        QTcpSocket *next = m_tcpServer->nextPendingConnection();
        next->abort();
        delete next;
    }
}

void BulkTransportCapacityMP::receiveRequest()
{
    LOG_INFO("New client request");

    // abort if received data is not what we expected
    if (m_tcpSocket->bytesAvailable() < (int)sizeof(quint64))
    {
        LOG_DEBUG("Data length is not what we expected");
        return;
    }

    // get bytes from message
    QDataStream in(m_tcpSocket);

    quint64 bytes;
    in >> bytes;
    sendResponse(bytes);
}

void BulkTransportCapacityMP::handleError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QAbstractSocket::RemoteHostClosedError)
    {
        return;
    }

    QAbstractSocket *socket = qobject_cast<QAbstractSocket *>(sender());
    emit error(QString("Socket Error: %1").arg(socket->errorString()));

    emit error(socket->errorString());
}

Measurement::Status BulkTransportCapacityMP::status() const
{
    return Unknown;
}

bool BulkTransportCapacityMP::prepare(NetworkManager *networkManager,
                                      const MeasurementDefinitionPtr &measurementDefinition)
{
    definition = measurementDefinition.dynamicCast<BulkTransportCapacityDefinition>();

    if (definition.isNull())
    {
        setErrorString("Definition is empty");
    }

    m_tcpSocket = 0;
    m_tcpServer = networkManager->createServerSocket();
    m_tcpServer->setParent(this);

    // Signal for errors
    connect(m_tcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this,
            SLOT(handleError(QAbstractSocket::SocketError)));

    // Signal for new clients
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(newClientConnection()));

    return true;
}

bool BulkTransportCapacityMP::stop()
{
    delete m_tcpSocket;
    return true;
}

Result BulkTransportCapacityMP::result() const
{
    return Result(errorString());
}
