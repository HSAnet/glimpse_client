#include "server.h"

#include <QTcpSocket>
#include <QDebug>

/*
// TODO: Is this a test case?
ClientHandler::ClientHandler(QTcpSocket *socket)
: socket(socket)
{
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void ClientHandler::disconnected()
{
    qDebug() << "Client connection from" << socket->peerAddress().toString() << socket->peerPort() << "closed";
    deleteLater();
}

void ClientHandler::readyRead()
{
    // Read all incoming data and write it back
    QByteArray data = socket->readAll();
    socket->write(data);
}*/

////////////////////////////////////////////////////////////////////////

/*
Server::Server(QObject *parent)
: QObject(parent)
{
    connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

Server::~Server()
{
}

bool Server::listen(quint16 port)
{
    // TODO: Check this code
    bool ok = server.listen(QHostAddress::Any, port);
    ok &= server.listen(QHostAddress::AnyIPv6, port);
    return ok;
}

void Server::newConnection()
{
    QTcpSocket* s = server.nextPendingConnection();

    QHostAddress remoteHost = s->peerAddress();
    qDebug() << "Incoming client connection from" << remoteHost.toString() << s->peerPort();

    // Assign a new client handler
    new ClientHandler(s);
}
*/
