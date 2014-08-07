#include "reverseDnslookup_plugin.h"
#include "reverseDnslookup.h"

QStringList ReverseDnslookupPlugin::measurements() const
{
    return QStringList() << "reversednslookup";
}

MeasurementPtr ReverseDnslookupPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new ReverseDnslookup);
}

MeasurementDefinitionPtr ReverseDnslookupPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return ReverseDnslookupDefinition::fromVariant(data);
}
