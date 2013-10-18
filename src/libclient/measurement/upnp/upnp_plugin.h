#ifndef UPNP_PLUGIN_H
#define UPNP_PLUGIN_H

#include "../measurementplugin.h"

class UPnPPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant &data);
};

#endif // UPNP_PLUGIN_H
