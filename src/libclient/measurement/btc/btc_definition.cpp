#include "btc_definition.h"

BulkTransportCapacityDefinition::BulkTransportCapacityDefinition(const QString &host, quint16 port,
                                                                 quint64 initialDataSize, quint16 slices)
: host(host)
, port(port)
, initialDataSize(initialDataSize)
, slices(slices)
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
    map.insert("initial_data_size", initialDataSize);
    map.insert("slices", slices);
    return map;
}

BulkTransportCapacityDefinitionPtr BulkTransportCapacityDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return BulkTransportCapacityDefinitionPtr(new BulkTransportCapacityDefinition(map.value("host", "").toString(),
                                                                                  map.value("port", 0).toUInt(),
                                                                                  map.value("initial_data_size", 1024 * 1024).toUInt(),
                                                                                  map.value("slices", 10).toUInt()));
}
