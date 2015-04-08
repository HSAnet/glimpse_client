#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H

#include "export.h"
#include <QHostAddress>

struct CLIENT_API RemoteHost
{
    bool isValid() const;

    QString host;
    quint16 port;
};

class CLIENT_API NetworkHelper
{
public:
    static bool isLinkLocalAddress(const QHostAddress &host);
    static bool isLocalIpAddress(const QHostAddress &host);

    static QHostAddress localIpAddress();
    static QHostAddress publicIpAddress();
    static QHostAddress gatewayIpAddress();
    static QList<QHostAddress> dnsServerIpAddresses();

    static RemoteHost remoteHost(const QString &hostname);
};

#endif // NETWORKHELPER_H
