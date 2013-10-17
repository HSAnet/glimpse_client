#ifndef BTC_PLUGIN_H
#define BTC_PLUGIN_H

#include "../measurementplugin.h"

class BulkTransportCapacityPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant &data);
};

#endif // BTC_PLUGIN_H
