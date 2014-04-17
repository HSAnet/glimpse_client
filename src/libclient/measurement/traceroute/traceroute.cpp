#include "../../log/logger.h"
#include "traceroute.h"

LOGGER("Traceroute");

Traceroute::Traceroute(QObject *parent) :
    Measurement(parent),
    currentStatus(Unknown),
    udpPing()
{
}

Traceroute::~Traceroute()
{
}

Measurement::Status Traceroute::status() const
{
    return currentStatus;
}

void Traceroute::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool Traceroute::prepare(NetworkManager* networkManager,
                         const MeasurementDefinitionPtr& measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<TracerouteDefinition>();

    connect(&udpPing, &UdpPing::destinationUnreachable,
            this, &Traceroute::destinationUnreachable);

    connect(&udpPing, &UdpPing::ttlExceeded,
            this, &Traceroute::ttlExceeded);

    return true;
}

bool Traceroute::start()
{
    return false;
}

bool Traceroute::stop()
{
    return true;
}

ResultPtr Traceroute::result() const
{
    QVariantList res;

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
}

void Traceroute::destinationUnreachable(PingProbe &probe)
{
    LOG_DEBUG("destination unreachable");
}

void Traceroute::ttlExceeded(PingProbe &probe)
{
    LOG_DEBUG("TTL exceeded");
}
