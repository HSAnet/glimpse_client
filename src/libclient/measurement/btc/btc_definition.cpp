#include "btc_definition.h"

BulkTransportCapacityDefinition::BulkTransportCapacityDefinition(QString host, quint16 port, quint64 initialDataSize)
    : host(host)
    , port(port)
    , initialDataSize(initialDataSize)
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
    return BulkTransportCapacityDefinitionPtr(new BulkTransportCapacityDefinition(map.value("host").toString(),
                                                                                  map.value("port").toUInt(),
                                                                                  map.value("initialDataSize").toUInt()));
}
