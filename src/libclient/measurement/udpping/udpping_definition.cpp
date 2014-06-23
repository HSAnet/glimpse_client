#include "udpping_definition.h"
#include "../../types.h"

UdpPingDefinition::UdpPingDefinition(const QString &url, const quint32 &count, const quint32 &interval,
                                     const quint32 &receiveTimeout,const int &ttl,
                                     const quint16 &destinationPort, const quint16 &sourcePort,
                                     const quint32 &payload, const QAbstractSocket::SocketType &pingType)
: url(url)
, count(count)
, interval(interval)
, receiveTimeout(receiveTimeout)
, ttl(ttl)
, destinationPort(destinationPort)
, sourcePort(sourcePort)
, payload(payload)
, pingType(pingType)
{

}

UdpPingDefinition::~UdpPingDefinition()
{

}

UdpPingDefinitionPtr UdpPingDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return UdpPingDefinitionPtr(new UdpPingDefinition(map.value("url", "").toString(),
                                                      map.value("count", 3).toUInt(),
                                                      map.value("interval", 1000).toUInt(),
                                                      map.value("receiveTimeout", 1000).toUInt(),
                                                      map.value("ttl", 64).toInt(),
                                                      map.value("destinationPort", 33434).toUInt(),
                                                      map.value("sourcePort", 33434).toUInt(),
                                                      map.value("payload", 74).toUInt(),
                                                      pingTypeFromString(map.value("pingType", "UdpSocket").toString())));
}

QVariant UdpPingDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("url", url);
    map.insert("count", count);
    map.insert("interval", interval);
    map.insert("receiveTimeout", receiveTimeout);
    map.insert("ttl", ttl);
    map.insert("destinationPort", destinationPort);
    map.insert("sourcePort", sourcePort);
    map.insert("payload", payload);
    map.insert("pingType", pingTypeToString(pingType));
    return map;
}
