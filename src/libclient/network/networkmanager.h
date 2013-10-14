#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "../task/task.h"

#include <QAbstractSocket>

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
        UdpSocket
//        UtpSocket
    };

    QAbstractSocket* createConnection(const TestDefinitionPtr& testDefinition, SocketType socketType);

protected:
    class Private;
    Private* d;
};

#endif // NETWORKMANAGER_H
