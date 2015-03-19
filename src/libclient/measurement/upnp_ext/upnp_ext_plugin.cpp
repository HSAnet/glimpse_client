#include "upnp_ext_plugin.h"
#include "upnp_ext.h"

QStringList UPnPPlugin::measurements() const
{
    return QStringList() << "upnp";
}

MeasurementPtr UPnPPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new UPnP);
}

MeasurementDefinitionPtr UPnPPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    Q_UNUSED(data);
    return MeasurementDefinitionPtr();
}
