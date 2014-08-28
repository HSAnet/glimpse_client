#include "ping_definition.h"

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
