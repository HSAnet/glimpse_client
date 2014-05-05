#include "traceroute_definition.h"

TracerouteDefinition::TracerouteDefinition(const QString &url,
                                           const quint32 &count,
                                           const quint32 &interval,
                                           const quint32 &receiveTimeout,
                                           const quint16 &destinationPort,
                                           const quint16 &sourcePort,
                                           const quint32 &payload)
: url(url),
count(count),
interval(interval),
receiveTimeout(receiveTimeout),
destinationPort(destinationPort),
sourcePort(sourcePort),
payload(payload)
{
}

TracerouteDefinition::~TracerouteDefinition()
{
}

TracerouteDefinitionPtr TracerouteDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return TracerouteDefinitionPtr(new TracerouteDefinition(
                            map.value("url", "").toString(),
                            map.value("count", 3).toUInt(),
                            map.value("interval", 1000).toUInt(),
                            map.value("receiveTimeout", 1000).toUInt(),
                            map.value("destinationPort", 33434).toUInt(),
                            map.value("sourcePort", 33434).toUInt(),
                            map.value("payload", 74).toUInt()));
}

QVariant TracerouteDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("url", url);
    map.insert("count", count);
    map.insert("interval", interval);
    map.insert("receiveTimeout", receiveTimeout);
    map.insert("destinationPort", destinationPort);
    map.insert("sourcePort", sourcePort);
    map.insert("payload", payload);
    return map;
}
