#ifndef Dnslookup_PLUGIN_H
#define Dnslookup_PLUGIN_H

#include "../measurementplugin.h"

class DnslookupPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant &data);
};

#endif // Dnslookup_PLUGIN_H
