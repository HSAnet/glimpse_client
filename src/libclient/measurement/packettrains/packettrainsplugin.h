#ifndef PACKETTRAINSPLUGIN_H
#define PACKETTRAINSPLUGIN_H

#include "../measurementplugin.h"

class PacketTrainsPlugin : public MeasurementPlugin
{
public:
    // MeasurementPlugin interface
    QStringList measurements() const;

    MeasurementPtr createMeasurement(const QString &name);
    MeasurementDefinitionPtr createMeasurementDefinition(const QString &name, const QVariant &data);
};

#endif // PACKETTRAINSPLUGIN_H
