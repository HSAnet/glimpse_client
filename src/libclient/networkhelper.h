#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H

#include <QHostAddress>

class NetworkHelper
{
public:
    static bool isLinkLocalAddress(const QHostAddress& host);
    static bool isLocalIpAddress(const QHostAddress& host);

    static QHostAddress localIpAddress();
};

#endif // NETWORKHELPER_H
