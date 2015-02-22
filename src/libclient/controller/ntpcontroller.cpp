#include <QTimer>
#include <QHostInfo>

#include "ntpcontroller.h"
#include "../log/logger.h"

LOGGER(NtpController);

static const int interval = 3600000;

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
    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readResponse()));
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

    memset(&packet, 0, sizeof(packet));

    if (m_socket->hasPendingDatagrams())
    {
        m_socket->readDatagram((char *)&packet, sizeof(packet), &address,
                               &port);

        m_localTime = QDateTime::currentDateTime();
        m_networkTime = NtpTimestamp::toDateTime(packet.receiveTimestamp);
    }
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
    return QDateTime::currentDateTime().addSecs(this->offset());
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
