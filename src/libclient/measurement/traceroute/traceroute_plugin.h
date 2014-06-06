#ifndef TRACEROUTE_PLUGIN_H
#define TRACEROUTE_PLUGIN_H

#include "../measurement.h"
#include "../measurementdefinition.h"
#include "../measurementplugin.h"

class TraceroutePlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString &name, const QVariant &data);
};

#endif // TRACEROUTE_PLUGIN_H
