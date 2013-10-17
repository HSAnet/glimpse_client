#include "tcpsocket.h"

class TcpSocket::Private : public QObject
{
    Q_OBJECT

public:
    Private(TcpSocket* q)
    : q(q)
    , bytesRead(0)
    , bytesWrite(0)
    {
        connect(q, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
        connect(q, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }

    TcpSocket* q;

    qint64 bytesRead;
    qint64 bytesWrite;

public slots:
    void bytesWritten(qint64 bytes);
    void readyRead();
};


void TcpSocket::Private::bytesWritten(qint64 bytes)
{
    bytesWrite += bytes;
}

void TcpSocket::Private::readyRead()
{
    bytesRead += q->bytesAvailable();
}

TcpSocket::TcpSocket(QObject *parent)
: QTcpSocket(parent)
, d(new Private(this))
{
}

TcpSocket::~TcpSocket()
{
    delete d;
}

qint64 TcpSocket::bytesWrite() const
{
    return d->bytesWrite;
}

qint64 TcpSocket::bytesRead() const
{
    return d->bytesRead;
}

#include "tcpsocket.moc"
