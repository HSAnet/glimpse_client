#ifndef MEASUREMENTPLUGIN_H
#define MEASUREMENTPLUGIN_H

#include "measurement.h"
#include "measurementdefinition.h"

#include <QStringList>

class CLIENT_API MeasurementPlugin
{
public:
    virtual ~MeasurementPlugin() {}

    virtual QStringList measurements() const = 0;

    virtual MeasurementPtr createMeasurement(const QString& name) = 0;
    virtual MeasurementDefinitionPtr createMeasurementDefinition(const QString& name, const QVariant& data) = 0;
};

#endif // MEASUREMENTPLUGIN_H
