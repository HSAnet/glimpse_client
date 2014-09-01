#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "../task/task.h"

#include <QAbstractSocket>
#include <QNetworkConfigurationManager>

class Scheduler;
class Settings;
class QUdpSocket;
class QTcpServer;

// FIXME: This class has to be thread safe!
class CLIENT_API NetworkManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(SocketType)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)

public:
    explicit NetworkManager(QObject *parent = 0);
    ~NetworkManager();

    bool init(Scheduler *scheduler, Settings *settings);

    void setRunning(bool running);
    bool isRunning() const;

    bool onMobileConnection();
    QList<QNetworkConfiguration::BearerType> connectionType();

    enum SocketType
    {
        TcpSocket,
        UdpSocket,
        UtpSocket
    };

    // Finds an existing connection
    QAbstractSocket *connection(const QString &hostname, SocketType socketType) const;

    // Creates a new connection
    QAbstractSocket *createConnection(SocketType socketType);
    QAbstractSocket *establishConnection(const QString &hostname,
                                         const quint32 &taskId,
                                         const QString &measurement,
                                         MeasurementDefinitionPtr measurementDefinition,
                                         NetworkManager::SocketType socketType);

    QTcpServer *createServerSocket();

signals:
    void runningChanged();

protected:
    class Private;
    Private *d;
};

#endif // NETWORKMANAGER_H
