#include "negotiator.h"

#include <QList>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

#define RANDOM_BETWEEN(min, max) (rand() % (max-min+1)) + min

class Negotiator::Private : public QObject
{
    Q_OBJECT

public:
    Private(Negotiator* q)
    : q(q)
    , managerSocket(NULL)
    {
        answerTimer.setSingleShot(true);
        answerTimer.setInterval(5000);
        connect(&answerTimer, SIGNAL(timeout()), this, SLOT(timeout()));

        qsrand(QDateTime::currentMSecsSinceEpoch());
    }

    Negotiator* q;

    // Properties
    QAbstractSocket* managerSocket;
    QList<QUdpSocket*> sockets;

    QTimer answerTimer;

    // Functions
    void processDatagram(const QByteArray& datagram);

public slots:
    void readyRead();
    void timeout();
};

void Negotiator::Private::processDatagram(const QByteArray &datagram)
{
    qDebug() << "Negotiator received datagram:" << datagram;

    emit q->finished();
}

void Negotiator::Private::readyRead()
{
    QUdpSocket* socket = qobject_cast<QUdpSocket*>(sender());
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.reserve(socket->pendingDatagramSize());

        socket->readDatagram(datagram.data(), datagram.size());

        processDatagram(datagram);
    }
}

void Negotiator::Private::timeout()
{
    qDebug() << "Negotiaton timed out";
    emit q->error();
}

Negotiator::Negotiator(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

Negotiator::~Negotiator()
{
    delete d;
}

void Negotiator::setManagerSocket(QAbstractSocket *managerSocket)
{
    d->managerSocket = managerSocket;
}

QAbstractSocket *Negotiator::managerSocket() const
{
    return d->managerSocket;
}

void Negotiator::sendRequest(const QHostAddress &address, quint16 port)
{
    qDeleteAll(d->sockets);

    // Send a request packet with these ports
    const quint32 magic = 0x1337;
    const quint8 version = 0;

    struct NegotiationPacket {
        quint16 magic;
        quint8 version;

        char host[64];
        quint16 port;

        quint16 ports[10];
    };

    NegotiationPacket packet;
    packet.magic = magic;
    packet.version = version;
    qstrcpy(packet.host, address.toString().toLatin1());
    packet.port = port;

    // Bind random ports
    for(int i=0; i < 10; ++i) {
        int tempPort = RANDOM_BETWEEN(8000, 8100);

        QUdpSocket* s = new QUdpSocket(this);
        connect(s, SIGNAL(readyRead()), d, SLOT(readyRead()));

        if ( !s->bind(tempPort) ) {
            delete s;

            qDebug() << "Binding port" << tempPort << "failed, ignoring.";
            continue;
        }

        // Don't care about unset ports in packet
        packet.ports[i] = tempPort;
        d->sockets.append(s);
    }

    // TODO: What to do when this packet is missing in action?
    d->answerTimer.start();
    d->managerSocket->write((const char*)&packet, sizeof(packet));
}

#include "negotiator.moc"
