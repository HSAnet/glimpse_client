#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "../export.h"

#include <QTcpSocket>

class CLIENT_API TcpSocket : public QTcpSocket
{
    Q_OBJECT

public:
    TcpSocket(QObject* parent = 0);
    ~TcpSocket();

    qint64 bytesWrite() const;
    qint64 bytesRead() const;

protected:
    class Private;
    Private* d;
};

#endif // TCPSOCKET_H
