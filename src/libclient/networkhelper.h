#ifndef NETWORKHELPER_H
#define NETWORKHELPER_H

#include "export.h"
#include <QHostAddress>

class CLIENT_API NetworkHelper
{
public:
    static bool isLinkLocalAddress(const QHostAddress& host);
    static bool isLocalIpAddress(const QHostAddress& host);

    static QHostAddress localIpAddress();
};

#endif // NETWORKHELPER_H
