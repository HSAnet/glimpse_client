#ifndef MEASUREMENTFACTORY_H
#define MEASUREMENTFACTORY_H

#include "measurement.h"

class CLIENT_API MeasurementFactory
{
public:
    MeasurementFactory();
    ~MeasurementFactory();

    MeasurementPtr createMeasurement(const QString& name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant& data);

protected:
    class Private;
    Private* d;
};

#endif // MEASUREMENTFACTORY_H
