#ifndef UDPPING_PLUGIN_H
#define UDPPING_PLUGIN_H

#include "../measurement.h"
#include "../measurementdefinition.h"
#include "../measurementplugin.h"

class UdpPingPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant &data);
};

#endif // UDPPING_PLUGIN_H
