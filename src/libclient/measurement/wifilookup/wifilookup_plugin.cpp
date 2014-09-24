#include "wifilookup_plugin.h"
#include "wifilookup.h"
#include "wifilookup_definition.h"

QStringList WifiLookupPlugin::measurements() const
{
    return QStringList()
           << "wifilookup";
}

MeasurementPtr WifiLookupPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new WifiLookup);
}

MeasurementDefinitionPtr WifiLookupPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return WifiLookupDefinition::fromVariant(data);
}
