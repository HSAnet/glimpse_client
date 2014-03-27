#include "udpping.h"

UdpPing::UdpPing(QObject *parent)
    : Measurement(parent),
      currentStatus(Unknown)
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

    foreach(struct pingProbe probe, m_pingProbes)
    {
        if (probe.recvTime > 0 && probe.recvTime > 0)
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

bool UdpPing::sendData(struct pingProbe &probe)
{
    return false;
}

void UdpPing::receiveData(struct pingProbe &probe)
{
}

void UdpPing::ping(struct pingProbe &probe)
{
    // send
    if (sendData(probe))
    {
        receiveData(probe);
    }
}

int UdpPing::getAddress(QString address, sockaddr_any *addr) const
{
    return 0;
}

// vim: set sts=4 sw=4 et:
