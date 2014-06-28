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

    MeasurementPtr createMeasurement(const QString &name, const quint32 &id);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString &name, const QVariant &data);

protected:
    class Private;
    Private *d;

private:
    Q_DISABLE_COPY(MeasurementFactory)
};

#endif // MEASUREMENTFACTORY_H
