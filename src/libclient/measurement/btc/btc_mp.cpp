#include "btc_mp.h"
#include <iostream>
#include <iomanip>
#include <QDataStream>
using namespace std;

BulkTransportCapacityMP::BulkTransportCapacityMP(QObject *parent)
: QObject(parent)
{
    m_tcpSocket = 0;
    m_tcpServer = new QTcpServer(this);

    // Signal for errors
    connect(m_tcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));

    // Signal for new clients
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(newClientConnection()));
}

bool BulkTransportCapacityMP::start()
{

    // Start listening
    return m_tcpServer->listen(QHostAddress::Any, definition->port);
}

qint64 BulkTransportCapacityMP::sendResponse(quint64 bytes)
{
    // send data back
    QByteArray data;
    data.resize(bytes);
    data.fill('X'); // TODO do we need to randomize this to prevent compression on the path?
    return m_tcpSocket->write(data);
}

void BulkTransportCapacityMP::resetServer()
{
    m_tcpSocket = 0;
}

void BulkTransportCapacityMP::newClientConnection()
{
    cout<<"New client connection"<<endl;

    if (!m_tcpSocket)
    {
        m_tcpSocket = m_tcpServer->nextPendingConnection();

        connect(m_tcpSocket, SIGNAL(disconnected()), m_tcpSocket, SLOT(deleteLater()));
        connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveRequest()));
        connect(m_tcpSocket, SIGNAL(destroyed()), this, SLOT(resetServer()));
    }
    else
    {
        cout<<"There is already a client connected, abort"<<endl;
        m_tcpServer->nextPendingConnection()->abort();
    }
}

void BulkTransportCapacityMP::receiveRequest()
{
    cout<<"New client request"<<endl;
    // get bytes from message
    QDataStream in(m_tcpSocket);
    quint64 bytes;

    // abort if received data is not what we expected
    if (m_tcpSocket->bytesAvailable() < (int)sizeof(quint64))
    {
        cout<<"Data length is not what we expected"<<endl;
        return;
    }

    in>>bytes;
    sendResponse(bytes);
}

void BulkTransportCapacityMP::handleError(QAbstractSocket::SocketError socketError)
{
    if(socketError == QAbstractSocket::RemoteHostClosedError)
        return;

    QAbstractSocket* socket = qobject_cast<QAbstractSocket*>(sender());
    cout<<"Socket Error: "<<socket->errorString().toStdString()<<endl;
}


Measurement::Status BulkTransportCapacityMP::status() const
{
}

bool BulkTransportCapacityMP::prepare(const MeasurementDefinitionPtr &measurementDefinition)
{
    definition = measurementDefinition.dynamicCast<BulkTransportCapacityDefinition>();
    return true;
}

bool BulkTransportCapacityMP::stop()
{
    return true;
}

ResultPtr BulkTransportCapacityMP::result() const
{
    return ResultPtr();
}
