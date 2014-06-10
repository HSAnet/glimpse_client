#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "../export.h"

#include <QUdpSocket>

class CLIENT_API UdpSocket : public QUdpSocket
{
    Q_OBJECT

public:
    UdpSocket(QObject *parent = 0);
    ~UdpSocket();
};

#endif // UDPSOCKET_H
