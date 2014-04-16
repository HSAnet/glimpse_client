#include "udpping.h"

namespace
{
    bool getAddress(const QString &address, sockaddr_any *addr)
    {
        Q_UNUSED(address);
        Q_UNUSED(addr);
        return true;
    }
}

UdpPing::UdpPing(QObject *parent)
: Measurement(parent)
, currentStatus(Unknown)
, m_device(NULL)
, m_capture(NULL)
, m_destAddress()
, m_payload(NULL)
{
}

UdpPing::~UdpPing()
{
}

Measurement::Status UdpPing::status() const
{
    return currentStatus;
}

void UdpPing::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool UdpPing::prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<UdpPingDefinition>();
    return true;
}

bool UdpPing::start()
{
    return false;
}

bool UdpPing::stop()
{
    return true;
}

ResultPtr UdpPing::result() const
{
    QVariantList res;

    foreach (const PingProbe &probe, m_pingProbes)
    {
        if (probe.sendTime > 0 && probe.recvTime > 0)
        {
            res << probe.recvTime - probe.sendTime;
        }
    }

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
}

int UdpPing::initSocket()
{
    return 0;
}

bool UdpPing::sendData(PingProbe *probe)
{
    Q_UNUSED(probe);
    return false;
}

void UdpPing::receiveData(PingProbe *probe)
{
    Q_UNUSED(probe);
}

void UdpPing::ping(PingProbe *probe)
{
    // send
    if (sendData(probe))
    {
        receiveData(probe);
    }
}

// vim: set sts=4 sw=4 et:
