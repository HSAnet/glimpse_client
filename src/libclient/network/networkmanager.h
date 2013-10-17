#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "../task/task.h"

#include <QAbstractSocket>

class QTcpServer;

// FIXME: This class has to be thread safe!
class CLIENT_API NetworkManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(SocketType)

public:
    explicit NetworkManager(QObject* parent = 0);
    ~NetworkManager();

    enum SocketType
    {
        TcpSocket,
        UdpSocket,
        UtpSocket
    };

    // Finds an existing connection
    QAbstractSocket* connection(const QString& hostname, SocketType socketType) const;

    // Creates a new connection
    QAbstractSocket* createConnection(const QString& hostname, SocketType socketType);
    QAbstractSocket* createConnection(const TestDefinitionPtr& testDefinition, SocketType socketType);

    QTcpServer* createServerSocket();

protected:
    class Private;
    Private* d;
};

#endif // NETWORKMANAGER_H
