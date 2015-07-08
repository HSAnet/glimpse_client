#include <QTimer>
#include <QHostInfo>

#include "ntpcontroller.h"
#include "../log/logger.h"

LOGGER(NtpController);

static const int interval = 3600000;

struct NtpTimestamp
{
    quint32 seconds;
    quint32 fraction;

    static QDateTime toDateTime(const NtpTimestamp &timestamp)
    {
        quint32 seconds = qFromBigEndian(timestamp.seconds);
        quint32 fraction = qFromBigEndian(timestamp.fraction);
        qint64 ms = seconds * 1000ll + fraction * 1000ll / 0x100000000ll;

        return QDateTime::fromMSecsSinceEpoch(ms - 2208988800000ll);
    }

    static NtpTimestamp fromDateTime(const QDateTime &timestamp)
    {
        NtpTimestamp result;
        qint64 ms = timestamp.toMSecsSinceEpoch() + 2208988800000ll;
        quint32 seconds = ms / 1000;
        quint32 fraction = 0x100000000ll * (ms % 1000) / 1000;

        result.seconds = qToBigEndian(seconds);
        result.fraction = qToBigEndian(fraction);

        return result;
    }
};

struct NtpPacket
{
    struct NtpFlags
    {
        quint8 mode: 3;
        quint8 versionNumber: 3;
        quint8 leapIndicator: 2;
    } flags;

    quint8 peerClockStratum;
    quint8 peerPollingInterval;
    quint8 peerClockPrecision;

    quint32 rootDelay;
    quint32 rootDispersion;
    quint32 referenceId;

    NtpTimestamp referenceTimestamp;
    NtpTimestamp originTimestamp;
    NtpTimestamp receiveTimestamp;
    NtpTimestamp transmitTimestamp;
};

class NtpController::Private : public QObject
{
    Q_OBJECT

public:
    Private(NtpController *q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), q, SLOT(update()));
        timer.start(interval);
    }

    NtpController *q;
    QTimer timer;
};

NtpController::NtpController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

NtpController::~NtpController()
{
    delete d;
}

void NtpController::update()
{
    QHostInfo hostInfo = QHostInfo::fromName("ptbtime1.ptb.de");

    if (!hostInfo.addresses().isEmpty())
    {
        QHostAddress ntpServer = hostInfo.addresses().first();
        sync(ntpServer);
    }
    else
    {
        LOG_WARNING("could not resolve ntp server");
    }
}

Controller::Status NtpController::status() const
{
    return Controller::Unknown;
}

QString NtpController::errorString() const
{
    return "";
}

bool NtpController::init()
{
    update();

    return true;
}

bool NtpController::sync(QHostAddress &host)
{
    NtpPacket packet;

    memset(&packet, 0, sizeof(packet));
    packet.flags.mode = 3;
    packet.flags.versionNumber = 4;
    packet.transmitTimestamp = NtpTimestamp::fromDateTime(
                                   QDateTime::currentDateTimeUtc());

    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readResponse()));

    if (m_socket->writeDatagram((char *)&packet, sizeof(packet), host, 123) < 0)
    {
        emit error("writeDatagram");
        return false;
    }

    return true;
}

void NtpController::readResponse()
{
    NtpPacket packet;
    QHostAddress address;
    quint16 port;
    qint64 bytes;

    memset(&packet, 0, sizeof(packet));

    while (m_socket->hasPendingDatagrams())
    {
        bytes = m_socket->readDatagram((char *)&packet, sizeof(packet), &address,
                               &port);

        m_localTime = QDateTime::currentDateTimeUtc();

        if (bytes == 48 && packet.receiveTimestamp.seconds > 0)
        {
            m_networkTime = NtpTimestamp::toDateTime(packet.receiveTimestamp);
        }
    }

    m_socket->close();
    delete m_socket;
}

QDateTime NtpController::localTime() const
{
    return m_localTime;
}

QDateTime NtpController::networkTime() const
{
    return m_networkTime;
}

QDateTime NtpController::currentDateTime() const
{
    return QDateTime::currentDateTimeUtc().addSecs(this->offset());
}

quint64 NtpController::offset() const
{
    if (!m_localTime.isValid() || !m_networkTime.isValid())
    {
        return 0;
    }

    return m_localTime.secsTo(m_networkTime);
}

#include "ntpcontroller.moc"
