#include "ping_definition.h"

PingDefinition::PingDefinition(const QString &host, quint16 count, quint32 timeout, quint32 interval)
: host(host)
, count(count)
, timeout(timeout)
, interval(interval)
{
}

PingDefinition::~PingDefinition()
{
}

QVariant PingDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("host", host);
    map.insert("count", count);
    map.insert("timeout", timeout);
    map.insert("interval", interval);
    return map;
}

PingDefinitionPtr PingDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return PingDefinitionPtr(new PingDefinition(map.value("host", "").toString(),
                                                map.value("count", 4).toUInt(),
                                                map.value("timeout", 2000).toUInt(),
                                                map.value("interval", 200).toUInt()));
}
