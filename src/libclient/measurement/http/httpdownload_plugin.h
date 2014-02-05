#ifndef HTTPDOWNLOAD_PLUGIN_H
#define HTTPDOWNLOAD_PLUGIN_H

#include "../measurementplugin.h"

class HTTPDownloadPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant &data);
};

#endif // HTTPDOWNLOAD_PLUGIN_H
