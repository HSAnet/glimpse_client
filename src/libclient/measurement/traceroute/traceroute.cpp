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
    mutex()
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

    if (!mutex.tryLock(0))
    {
        mutex.unlock();
        mutex.lock();
    }

    return true;
}

bool Traceroute::start()
{
    traceroute();
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

void Traceroute::traceroute()
{
    // TODO: remove hard-coded maximum hops
    for (quint32 i = 1; i < 30; i++)
    {
        ping(i);
        mutex.lock();
        if (hops.last().response == DESTINATION_UNREACHABLE)
        {
            break;
        }
    }

    emit finished();
}

void Traceroute::ping(quint32 ttl)
{
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
    Hop hop = {probe, DESTINATION_UNREACHABLE};
    hops << hop;
    mutex.unlock();
}

void Traceroute::ttlExceeded(PingProbe &probe)
{
    Hop hop = {probe, TTL_EXCEEDED};
    hops << hop;
    mutex.unlock();
}

void Traceroute::timeout(PingProbe &probe)
{
    Hop hop = {probe, TIMEOUT};
    hops << hop;
    mutex.unlock();
}
