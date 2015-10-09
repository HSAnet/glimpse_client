#include "upnp_plugin.h"
#include "upnp.h"

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
    return UPnPDefinition::fromVariant(data);
}
