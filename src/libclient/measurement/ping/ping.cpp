#include "ping.h"

namespace
{
    bool getAddress(const QString &address, sockaddr_any *addr)
    {
        Q_UNUSED(address);
        Q_UNUSED(addr);
        return true;
    }
}

Ping::Ping(QObject *parent)
: Measurement(parent)
, currentStatus(Unknown)
, m_device(NULL)
, m_capture(NULL)
, m_destAddress()
{
}

Ping::~Ping()
{
}

Measurement::Status Ping::status() const
{
    return currentStatus;
}

void Ping::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool Ping::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<PingDefinition>();
    return true;
}

bool Ping::start()
{
    setStartDateTime(QDateTime::currentDateTime());

    return false;
}

bool Ping::stop()
{
    return true;
}

ResultPtr Ping::result() const
{
    QVariantList res;

    foreach (const PingProbe &probe, m_pingProbes)
    {
        if (probe.sendTime > 0 && probe.recvTime > 0)
        {
            res << probe.recvTime - probe.sendTime;
        }
    }

    return ResultPtr(new Result(startDateTime(), QDateTime::currentDateTime(), res, QVariant()));
}

int Ping::initSocket()
{
    return 0;
}

bool Ping::sendData(PingProbe *probe)
{
    Q_UNUSED(probe);
    return false;
}

void Ping::receiveData(PingProbe *probe)
{
    Q_UNUSED(probe);
}

void Ping::ping(PingProbe *probe)
{
    // send
    if (sendData(probe))
    {
        receiveData(probe);
    }
}

// vim: set sts=4 sw=4 et:
