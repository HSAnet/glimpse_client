#include "extendedupnp_plugin.h"
#include "extendedupnp.h"

QStringList extendedUPnP_plugin::measurements() const
{
    return QStringList() << "extended_upnp";
}

MeasurementPtr extendedUPnP_plugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new extendedUPnP);
}

MeasurementDefinitionPtr extendedUPnP_plugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(data);
    Q_UNUSED(name);
    return MeasurementDefinitionPtr();
}
