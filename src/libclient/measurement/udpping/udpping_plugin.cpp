#include "udpping_plugin.h"
#include "udpping.h"
#include "udpping_definition.h"

QStringList UdpPingPlugin::measurements() const
{
    return QStringList()
           << "udpping";
}

MeasurementPtr UdpPingPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new UdpPing);
}

MeasurementDefinitionPtr UdpPingPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return UdpPingDefinition::fromVariant(data);
}
