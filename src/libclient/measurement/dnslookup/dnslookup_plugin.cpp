#include "ping_plugin.h"
#include "ping.h"

QStringList PingPlugin::measurements() const
{
    return QStringList() << "ping";
}

MeasurementPtr PingPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new Ping);
}

MeasurementDefinitionPtr PingPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return PingDefinition::fromVariant(data);
}
