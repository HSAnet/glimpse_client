#include "traceroute_definition.h"

TracerouteDefinition::TracerouteDefinition(const QString &host,
                                           const quint32 &count,
                                           const quint32 &interval,
                                           const quint32 &receiveTimeout,
                                           const quint16 &destinationPort,
                                           const quint16 &sourcePort,
                                           const quint32 &payload,
                                           const ping::PingType &type)
: host(host)
, count(count)
, interval(interval)
, receiveTimeout(receiveTimeout)
, destinationPort(destinationPort)
, sourcePort(sourcePort)
, payload(payload)
, type(type)
{
}

TracerouteDefinition::~TracerouteDefinition()
{
}

TracerouteDefinitionPtr TracerouteDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    if (map.isEmpty())
    {
        return TracerouteDefinitionPtr();
    }

    return TracerouteDefinitionPtr(new TracerouteDefinition(
                                       map.value("destination.url", "").toString(),
                                       map.value("packets.ip", 3).toUInt(),
                                       map.value("glimpse.ping.interval.ms", 1000).toUInt(),
                                       map.value("glimpse.ping.timeout.ms", 1000).toUInt(),
                                       map.value("destination.port", 33434).toUInt(),
                                       map.value("source.port", 33434).toUInt(),
                                       map.value("glimpse.ping.payload", 74).toUInt(),
                                       pingTypeFromString(map.value(
                                                              "glimpse.ping.type", "Udp").toString().toLatin1())));
}

QVariant TracerouteDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("host", host);
    map.insert("count", count);
    map.insert("interval", interval);
    map.insert("receive_timeout", receiveTimeout);
    map.insert("destination_port", destinationPort);
    map.insert("source_port", sourcePort);
    map.insert("payload", payload);
    map.insert("ping_type", pingTypeToString(type));
    return map;
}

QVariantMap TracerouteDefinition::capability()
{
    QVariantMap parameters;
    parameters.insert("destination.url", "*");
    parameters.insert("packets.ip", "*");
    parameters.insert("glimpse.ping.timeout.ms", "*");
    parameters.insert("glimpse.ping.interval.ms", "*");
    parameters.insert("destination.port", "*");
    parameters.insert("source.port", "*");
    parameters.insert("glimpse.ping.payload", "*");

    QVariantList results;
    results.append("glimpse.traceroute.start-time");
    results.append("glimpse.traceroute.hop-ip");
    results.append("ttl");
    results.append("rtt.ms.min");
    results.append("rtt.ms.max");
    results.append("rtt.ms.avg");
    results.append("rtt.ms.stdev");
    results.append("packets.ip");

    QVariantMap capability;
    capability.insert("capability", "measure");
    capability.insert("label", "glimpse-traceroute");
    capability.insert("parameters", parameters);
    capability.insert("results", results);
    capability.insert("version", 0);
    capability.insert("when", "now");
    capability.insert("registry", "https://www.measure-it.net/static/glimpse_registry.json");

    return capability;
}
