#include "packettrainsdefinition.h"


PacketTrainsDefinition::PacketTrainsDefinition(QString host, quint16 port, quint16 packetSize, quint16 trainLength,
                                               quint8 iterations, quint64 rateMin, quint64 rateMax, quint64 delay)
    : host(host)
    , port(port)
    , packetSize(packetSize)
    , trainLength(trainLength)
    , iterations(iterations)
    , rateMin(rateMin)
    , rateMax(rateMax)
    , delay(delay)
{

}

QVariant PacketTrainsDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("host", host);
    map.insert("port", port);
    map.insert("packetSize", packetSize);
    map.insert("trainLength", trainLength);
    map.insert("iterations", iterations);
    map.insert("rateMin", rateMin);
    map.insert("rateMax", rateMax);
    map.insert("delay", delay);
    return map;
}

PacketTrainsDefinitionPtr PacketTrainsDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return PacketTrainsDefinitionPtr(new PacketTrainsDefinition(map.value("host").toString(),
                                                                map.value("port").toUInt(),
                                                                map.value("packetSize").toUInt(),
                                                                map.value("trainLength").toUInt(),
                                                                map.value("iterations").toUInt(),
                                                                map.value("rateMin").toUInt(),
                                                                map.value("rateMax").toUInt(),
                                                                map.value("delay").toUInt()));
}
