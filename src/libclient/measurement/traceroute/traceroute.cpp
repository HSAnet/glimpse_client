#include "../../log/logger.h"
#include "traceroute.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <arpa/inet.h>
#elif defined(Q_OS_WIN)
#include <winsock2.h>
#endif

LOGGER("Traceroute");

Traceroute::Traceroute(QObject *parent)
: Measurement(parent)
, currentStatus(Unknown)
, m_ping()
, endOfRoute(false)
, ttl(0)
{
    setResultHeader(QStringList() << "results");
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

bool Traceroute::prepare(NetworkManager *networkManager,
                         const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<TracerouteDefinition>();

    if (definition->type != ping::Udp)
    {
        setErrorString("Ping type not supported");
        return false;
    }

    connect(&m_ping, SIGNAL(destinationUnreachable(const PingProbe &)),
            this, SLOT(destinationUnreachable(const PingProbe &)));

    connect(&m_ping, SIGNAL(ttlExceeded(const PingProbe &)),
            this, SLOT(ttlExceeded(const PingProbe &)));

    connect(&m_ping, SIGNAL(timeout(const PingProbe &)),
            this, SLOT(timeout(const PingProbe &)));

    connect(&m_ping, SIGNAL(udpResponse(const PingProbe &)), this,
            SLOT(udpResponse(const PingProbe &)));

    connect(&m_ping, SIGNAL(finished()), this, SLOT(pingFinished()));

    connect(&m_ping, SIGNAL(error(const QString &)), &m_ping,
            SLOT(setErrorString(const QString &)));

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

Result Traceroute::result() const
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

            probe.insert("response", hops[i + k].response);
            probe.insert("rtt", (int)(hops[i + k].probe.recvTime -
                                      hops[i + k].probe.sendTime));
            pings.append(probe);
        }

        hop.insert("hop", QString(inet_ntoa(hops[i].probe.source.sin.sin_addr)));
        hop.insert("pings", pings);
        hop.insert("ttl", i / 3 + 1);

        res << hop;
    }

    return Result(QVariantList() << QVariant(res));
}

void Traceroute::ping()
{
    if (++ttl == 30)
    {
        emit finished();
        return;
    }

    PingDefinition pingDef(definition->host,
                                 definition->count,
                                 definition->interval,
                                 definition->receiveTimeout,
                                 ttl,
                                 definition->destinationPort,
                                 definition->sourcePort,
                                 definition->payload,
                                 definition->type);

    if (m_ping.prepare(NULL, PingPlugin().createMeasurementDefinition(
                           "ping",
                           pingDef.toVariant())))
    {
        m_ping.start();
    }
}

void Traceroute::destinationUnreachable(const PingProbe &probe)
{
    Hop hop = {probe, traceroute::DESTINATION_UNREACHABLE};
    hops << hop;
    endOfRoute = true;
}

void Traceroute::ttlExceeded(const PingProbe &probe)
{
    Hop hop = {probe, traceroute::TTL_EXCEEDED};
    hops << hop;
}

void Traceroute::timeout(const PingProbe &probe)
{
    Hop hop = {probe, traceroute::TIMEOUT};
    hops << hop;
}

void Traceroute::udpResponse(const PingProbe &probe)
{
    Hop hop = {probe, traceroute::UDP_RESPONSE};
    hops << hop;
    endOfRoute = true;
}

void Traceroute::pingFinished()
{
    if (endOfRoute)
    {
        emit finished();
    }
    else
    {
        ping();
    }
}
