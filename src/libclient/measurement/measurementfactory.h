#ifndef MEASUREMENTFACTORY_H
#define MEASUREMENTFACTORY_H

#include "measurementplugin.h"

class CLIENT_API MeasurementFactory
{
public:
    MeasurementFactory();
    ~MeasurementFactory();

    MeasurementPluginList plugins() const;
    QStringList availableMeasurements() const;

    MeasurementPtr createMeasurement(const QString& name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant& data);

protected:
    class Private;
    Private* d;
};

#endif // MEASUREMENTFACTORY_H
