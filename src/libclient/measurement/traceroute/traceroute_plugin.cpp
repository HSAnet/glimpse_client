#include "traceroute_plugin.h"
#include "traceroute.h"
#include "traceroute_definition.h"

QStringList TraceroutePlugin::measurements() const
{
    return QStringList()
           << "traceroute";
}

MeasurementPtr TraceroutePlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new Traceroute);
}

MeasurementDefinitionPtr TraceroutePlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return TracerouteDefinition::fromVariant(data);
}
