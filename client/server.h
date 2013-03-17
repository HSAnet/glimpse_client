#ifndef SERVER_H
#define SERVER_H

#include <QUdpSocket>

/*
class ClientHandler : public QObject
{
    Q_OBJECT

public:
    ClientHandler(QTcpSocket* socket);

private slots:
    void disconnected();
    void readyRead();

protected:
    QTcpSocket* socket;
};
*/

/*class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = 0);
    ~Server();

    bool listen(quint16 port = 3543);

private slots:
    void newConnection();

protected:
    QTcpServer server;
};*/

#endif // SERVER_H
