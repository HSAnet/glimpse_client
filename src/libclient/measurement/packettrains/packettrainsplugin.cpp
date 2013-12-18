#include "packettrainsplugin.h"
#include "packettrains_ma.h"
#include "packettrains_mp.h"
#include "packettrainsdefinition.h"

QStringList PacketTrainsPlugin::measurements() const
{
    return QStringList()
            << "packettrains_ma"
            << "packettrains_mp";
}

MeasurementPtr PacketTrainsPlugin::createMeasurement(const QString &name)
{
    if (name == "packettrains_ma")
        return MeasurementPtr(new PacketTrainsMA);

    if (name == "packettrains_mp")
        return MeasurementPtr(new PacketTrainsMP);

    return MeasurementPtr();
}

MeasurementDefinitionPtr PacketTrainsPlugin::createMeasurementDefinition(const QString &name, const QVariant &data)
{
    Q_UNUSED(name);
    return PacketTrainsDefinition::fromVariant(data);
}
