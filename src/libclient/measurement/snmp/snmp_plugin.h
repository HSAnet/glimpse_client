#ifndef SNMPPLUGIN_H
#define SNMPPLUGIN_H

#include "measurement/measurementplugin.h"

class SnmpPlugin : public MeasurementPlugin
{
public:
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString &name, const QVariant &data);
};

#endif // SNMPPLUGIN_H
