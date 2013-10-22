#ifndef PING_PLUGIN_H
#define PING_PLUGIN_H

#include "../measurementplugin.h"

class PingPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant &data);
};

#endif // PING_PLUGIN_H
