#include "reverse_dnslookup_plugin.h"
#include "reverse_dnslookup.h"

QStringList ReverseDnslookupPlugin::measurements() const
{
    return QStringList() << "reverse_dnslookup";
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
