#include "snmp_plugin.h"
#include "snmp.h"


QStringList SnmpPlugin::measurements() const
{
    return QStringList() << "SNMP device scan";
}

MeasurementPtr SnmpPlugin::createMeasurement(const QString &name)
{
    Q_UNUSED(name);
    return MeasurementPtr(new Snmp);
}

MeasurementDefinitionPtr SnmpPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return SnmpDefinition::fromVariant(&data);
}

