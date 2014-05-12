#ifndef REVERSEDNSLOOKUP_PLUGIN_H
#define REVERSEDNSLOOKUP_PLUGIN_H

#include "../measurementplugin.h"

class ReverseDnslookupPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant &data);
};

#endif // REVERSEDNSLOOKUP_PLUGIN_H
