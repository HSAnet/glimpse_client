#ifndef CLIENT_H
#define CLIENT_H

#include "types.h"

#include <QObject>
#include <QLocale>
#include <QHostAddress>
#include <QStringList>

class QNetworkAccessManager;

////////////////////////////////////////////////////////////

class ClientInfo
{
public:
    PlatformType platformType;
    NetworkType networkType;
    QLocale locale;
};

////////////////////////////////////////////////////////////

class RemoteInfo
{
public:
    PlatformType platformType;
    QHostAddress peerAddress;
    quint16 peerPort;

    QStringList tests;
    Direction direction;
};

typedef QList<RemoteInfo> RemoteInfoList;

////////////////////////////////////////////////////////////

class Client : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)

    explicit Client(QObject *parent = 0);
    ~Client();

public:
    /// Receives the main client instance
    static Client* instance();

    enum Status
    {
        Unregistered,
        Registered
    };

    bool init();

    /// Get the current status
    Status status() const;

    QNetworkAccessManager* networkAccessManager() const;

    void setRemoteInfo(const RemoteInfoList& remoteInfo);
    RemoteInfoList remoteInfo() const;

public slots:
    void registerWithDiscovery();
    void register_();
    void unregister();

signals:
    void statusChanged();

private:
    class Private;
    Private* d;
};

#endif // CLIENT_H
