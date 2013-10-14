#ifndef MEASUREMENTFACTORY_H
#define MEASUREMENTFACTORY_H

#include "measurement.h"

class CLIENT_API MeasurementFactory
{
public:
    MeasurementPtr createMeasurement(const QString& name);
};

#endif // MEASUREMENTFACTORY_H
