#include "btc_definition.h"

BulkTransportCapacityDefinition::BulkTransportCapacityDefinition(const QString &host, quint16 port,
                                                                 quint64 initialDataSize)
: host(host)
, port(port)
, initialDataSize(initialDataSize)
{
}

BulkTransportCapacityDefinition::~BulkTransportCapacityDefinition()
{
}

QVariant BulkTransportCapacityDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("host", host);
    map.insert("port", port);
    map.insert("initialDataSize", initialDataSize);
    return map;
}

BulkTransportCapacityDefinitionPtr BulkTransportCapacityDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return BulkTransportCapacityDefinitionPtr(new BulkTransportCapacityDefinition(map.value("host", "").toString(),
                                                                                  map.value("port", 0).toUInt(),
                                                                                  map.value("initialDataSize", 1024 * 1024).toUInt()));
}
