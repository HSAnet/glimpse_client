#include "ping_definition.h"

#include <QVariantMap>

PingDefinition::PingDefinition(const QString &host, const quint32 &count, const quint32 &interval,
                                     const quint32 &receiveTimeout, const int &ttl,
                                     const quint16 &destinationPort, const quint16 &sourcePort,
                                     const quint32 &payload, const ping::PingType &type)
: host(host)
, count(count)
, interval(interval)
, receiveTimeout(receiveTimeout)
, ttl(ttl)
, destinationPort(destinationPort)
, sourcePort(sourcePort)
, payload(payload)
, type(type)
{

}

PingDefinition::~PingDefinition()
{

}

PingDefinitionPtr PingDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return PingDefinitionPtr(new PingDefinition(map.value("host", "").toString(),
                                                      map.value("count", 3).toUInt(),
                                                      map.value("interval", 1000).toUInt(),
                                                      map.value("timeout", 1000).toUInt(),
                                                      map.value("ttl", 64).toInt(),
                                                      map.value("destination_port", 33434).toUInt(),
                                                      map.value("source_port", 33434).toUInt(),
                                                      map.value("payload", 74).toUInt(),
                                                      pingTypeFromString(map.value(
                                                                             "type", "Udp").toString().toLatin1())));
}

PingDefinitionPtr PingDefinition::fromSpecification(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    if (map.isEmpty())
    {
        return PingDefinitionPtr();
    }

    return PingDefinitionPtr(new PingDefinition(map.value("destination.url", "").toString(),
                                                map.value("packets.ip", 3).toUInt(),
                                                map.value("glimpse.ping.interval.ms", 1000).toUInt(),
                                                map.value("glimpse.ping.timeout.ms", 1000).toUInt(),
                                                map.value("ttl", 64).toInt(),
                                                map.value("destination.port", 33434).toUInt(),
                                                map.value("source.port", 33434).toUInt(),
                                                map.value("glimpse.ping.payload", 74).toUInt(),
                                                pingTypeFromString(map.value(
                                                "glimpse.ping.type", "Udp").toString().toLatin1())));
}

QVariant PingDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("host", host);
    map.insert("count", count);
    map.insert("interval", interval);
    map.insert("timeout", receiveTimeout);
    map.insert("ttl", ttl);
    map.insert("destination_port", destinationPort);
    map.insert("source_port", sourcePort);
    map.insert("payload", payload);
    map.insert("type", pingTypeToString(type));
    return map;
}

QVariantMap PingDefinition::capability()
{
    QVariantMap parameters;
    parameters.insert("glimpse.ping.type", "*");
    parameters.insert("destination.url", "*");
    parameters.insert("packets.ip", "*");
    parameters.insert("glimpse.ping.timeout.ms", "*");
    parameters.insert("glimpse.ping.interval.ms", "*");
    parameters.insert("ttl", "*");
    parameters.insert("destination.port", "*");
    parameters.insert("source.port", "*");
    parameters.insert("glimpse.ping.payload", "*");

    QVariantList results;
    //results.append("rtt.ms");
    results.append("rtt.ms.min");
    results.append("rtt.ms.max");
    results.append("rtt.ms.avg");
    results.append("rtt.ms.stdev");
    results.append("packets.ip.sent");
    results.append("packets.ip.received");

    QVariantMap capability;
    capability.insert("capability", "measure");
    capability.insert("label", "glimpse-ping");
    capability.insert("parameters", parameters);
    capability.insert("results", results);
    capability.insert("version", 0);
    capability.insert("when", "now");
    capability.insert("registry", "https://www.measure-it.net/static/glimpse_registry.json");

    return capability;
}
