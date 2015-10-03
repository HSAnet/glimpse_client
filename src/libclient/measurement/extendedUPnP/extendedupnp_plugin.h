#ifndef EXTENDEDUPNP_PLUGIN_H
#define EXTENDEDUPNP_PLUGIN_H

#include "../measurementplugin.h"

class extendedUPnP_plugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString &name, const QVariant &data);
};

#endif // EXTENDEDUPNP_PLUGIN_H
