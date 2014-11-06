#include "../log/logger.h"
#include "ntp.h"

#include <cstring>

LOGGER("Ntp");

Ntp::Ntp(QObject *parent)
: QObject(parent)
{
    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readResponse()));
}

Ntp::~Ntp()
{
    return;
}

bool Ntp::sync(QHostAddress &host)
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

void Ntp::readResponse()
{
    NtpPacket packet;
    QHostAddress address;
    quint16 port;
    QDateTime localTime;
    QDateTime networkTime;

    memset(&packet, 0, sizeof(packet));

    if (m_socket->hasPendingDatagrams())
    {
        m_socket->readDatagram((char *)&packet, sizeof(packet), &address,
                               &port);

        m_localTime = QDateTime::currentDateTime();
        m_networkTime = NtpTimestamp::toDateTime(packet.receiveTimestamp);
    }
}

QDateTime Ntp::localTime() const
{
    return m_localTime;
}

QDateTime Ntp::networkTime() const
{
    return m_networkTime;
}

quint64 Ntp::offset() const
{
    if (!m_localTime.isValid() || !m_networkTime.isValid())
    {
        return 0;
    }

    return m_localTime.secsTo(m_networkTime);
}
