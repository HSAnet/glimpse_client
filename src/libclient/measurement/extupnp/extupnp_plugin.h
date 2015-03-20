#ifndef EXTUPNP_PLUGIN_H
#define EXTUPNP_PLUGIN_H

#include "../measurementplugin.h"

class ExtUPnPPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString &name, const QVariant &data);
};

#endif // EXTUPNP_PLUGIN_H
