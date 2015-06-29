#include "dnslookup_plugin.h"
#include "dnslookup.h"

QStringList DnslookupPlugin::measurements() const
{
    return QStringList() << "dnslookup";
}

MeasurementPtr DnslookupPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new Dnslookup);
}

MeasurementDefinitionPtr DnslookupPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return DnslookupDefinition::fromSpecification(data);
}
