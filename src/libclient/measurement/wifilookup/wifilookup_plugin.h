#ifndef WIFILOOKUP_PLUGIN_H
#define WIFILOOKUP_PLUGIN_H

#include "../measurement.h"
#include "../measurementdefinition.h"
#include "../measurementplugin.h"

class WifiLookupPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString &name, const QVariant &data);
};

#endif // WIFILOOKUP_PLUGIN_H
