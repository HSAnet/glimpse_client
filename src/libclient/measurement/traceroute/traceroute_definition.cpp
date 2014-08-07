#include "traceroute_definition.h"

TracerouteDefinition::TracerouteDefinition(const QString &host,
                                           const quint32 &count,
                                           const quint32 &interval,
                                           const quint32 &receiveTimeout,
                                           const quint16 &destinationPort,
                                           const quint16 &sourcePort,
                                           const quint32 &payload,
                                           const ping::PingType &pingType)
: host(host)
, count(count)
, interval(interval)
, receiveTimeout(receiveTimeout)
, destinationPort(destinationPort)
, sourcePort(sourcePort)
, payload(payload)
, pingType(pingType)
{
}

TracerouteDefinition::~TracerouteDefinition()
{
}

TracerouteDefinitionPtr TracerouteDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return TracerouteDefinitionPtr(new TracerouteDefinition(
                                       map.value("host", "").toString(),
                                       map.value("count", 3).toUInt(),
                                       map.value("interval", 1000).toUInt(),
                                       map.value("receive_timeout", 1000).toUInt(),
                                       map.value("destination_port", 33434).toUInt(),
                                       map.value("source_port", 33434).toUInt(),
                                       map.value("payload", 74).toUInt(),
                                       pingTypeFromString(map.value(
                                                              "pingType", "UdpSocket").toString().toLatin1())));
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
    map.insert("pingType", pingTypeToString(pingType));
    return map;
}
