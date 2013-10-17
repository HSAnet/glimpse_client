#include "btc_plugin.h"
#include "btc_ma.h"
#include "btc_mp.h"
#include "btc_definition.h"

QStringList BulkTransportCapacityPlugin::measurements() const
{
    return QStringList()
            << "btc_ma"
            << "btc_mp";
}

MeasurementPtr BulkTransportCapacityPlugin::createMeasurement(const QString &name)
{
    if (name == "btc_ma")
        return MeasurementPtr(new BulkTransportCapacityMA);

    if (name == "btc_mp")
        return MeasurementPtr(new BulkTransportCapacityMP);

    return MeasurementPtr();
}

MeasurementDefinitionPtr BulkTransportCapacityPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return BulkTransportCapacityDefinition::fromVariant(data);
}
