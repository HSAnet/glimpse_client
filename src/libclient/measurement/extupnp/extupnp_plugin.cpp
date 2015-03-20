#include "extupnp_plugin.h"
#include "extupnp.h"

QStringList ExtUPnPPlugin::measurements() const
{
    return QStringList() << "extupnp";
}

MeasurementPtr ExtUPnPPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new ExtUPnP);
}

MeasurementDefinitionPtr ExtUPnPPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    Q_UNUSED(data);
    return MeasurementDefinitionPtr();
}
