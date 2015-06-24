#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "../task/scheduledefinition.h"

#include <QAbstractSocket>
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
// mimick QNetworkInfo
namespace QNetworkInfo
{
    enum NetworkMode
    {
        UnknownMode,
        GsmMode,
        CdmaMode,
        WcdmaMode,
        WlanMode,
        EthernetMode,
        BluetoothMode,
        WimaxMode,
        LteMode,
        TdscdmaMode
    };
}
#else
#include <qnetworkinfo.h>
#endif

class Scheduler;
class Settings;
class QUdpSocket;
class QTcpServer;

// TODO: This class has to be thread safe!
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

    bool onMobileConnection() const;
    QNetworkInfo::NetworkMode connectionMode() const;

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
                                         const TaskId &taskId,
                                         const QString &measurement,
                                         MeasurementDefinitionPtr measurementDefinition,
                                         NetworkManager::SocketType socketType);

    QTcpServer *createServerSocket();
    bool allInterfacesDown() const;

signals:
    void runningChanged();

protected:
    class Private;
    Private *d;
};

#endif // NETWORKMANAGER_H
