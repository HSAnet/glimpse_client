#include "../../log/logger.h"
#include "traceroute.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <arpa/inet.h>
#elif defined(Q_OS_WIN)
#include <winsock2.h>
#endif

LOGGER("Traceroute");

Traceroute::Traceroute(QObject *parent) :
    Measurement(parent),
    currentStatus(Unknown),
    udpPing(),
    receivedDestinationUnreachable(false),
    ttl(0)
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

    connect(&udpPing, SIGNAL(destinationUnreachable(PingProbe&)),
            this, SLOT(destinationUnreachable(PingProbe&)));

    connect(&udpPing, SIGNAL(ttlExceeded(PingProbe&)),
            this, SLOT(ttlExceeded(PingProbe&)));

    connect(&udpPing, SIGNAL(timeout(PingProbe&)),
            this, SLOT(timeout(PingProbe&)));

    connect(&udpPing, SIGNAL(finished()), this, SLOT(pingFinished()));

    return true;
}

bool Traceroute::start()
{
    ping();

    return true;
}

bool Traceroute::stop()
{
    return true;
}

ResultPtr Traceroute::result() const
{
    QVariantList res;
    QVariantList pings;
    QVariantMap hop;
    QVariantMap probe;

    for (int i = 0; i < hops.size(); i += definition->count)
    {
        pings.clear();

        for (quint32 k = 0; k < definition->count; k++)
        {
            if ((quint32) hops.size() <= i + k)
            {
                break;
            }

            probe.insert("reponse", hops[i + k].response);
            probe.insert("rtt", (int) (hops[i + k].probe.recvTime -
                                       hops[i + k].probe.sendTime));
            pings.append(probe);
        }

        hop.insert("hop", QString(inet_ntoa(hops[i].probe.source.sin.sin_addr)));
        hop.insert("pings", pings);
        hop.insert("ttl", i / 3 + 1);

        res << hop;
    }

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
}

void Traceroute::ping()
{
    if (++ttl == 30)
    {
        emit finished();
        return;
    }

    UdpPingDefinition udpPingDef(definition->url,
                                 definition->count,
                                 definition->interval,
                                 definition->receiveTimeout,
                                 ttl,
                                 definition->destinationPort,
                                 definition->sourcePort,
                                 definition->payload);
    udpPing.prepare(NULL, UdpPingPlugin().createMeasurementDefinition(
                "udpping",
                udpPingDef.toVariant()));
    udpPing.start();
}

void Traceroute::destinationUnreachable(PingProbe &probe)
{
    Hop hop = {probe, traceroute::DESTINATION_UNREACHABLE};
    hops << hop;
    receivedDestinationUnreachable = true;
}

void Traceroute::ttlExceeded(PingProbe &probe)
{
    Hop hop = {probe, traceroute::TTL_EXCEEDED};
    hops << hop;
}

void Traceroute::timeout(PingProbe &probe)
{
    Hop hop = {probe, traceroute::TIMEOUT};
    hops << hop;
}

void Traceroute::pingFinished()
{
    if (receivedDestinationUnreachable)
    {
        emit finished();
    }
    else
    {
        ping();
    }
}
