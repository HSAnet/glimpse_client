#include "ping_plugin.h"
#include "ping.h"
#include "ping_definition.h"

QStringList PingPlugin::measurements() const
{
    return QStringList()
           << "ping";
}

MeasurementPtr PingPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new Ping);
}

MeasurementDefinitionPtr PingPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return PingDefinition::fromSpecification(data);
}
