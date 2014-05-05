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
    return PacketTrainsDefinitionPtr(new PacketTrainsDefinition(map.value("host", "").toString(),
                                                                map.value("port", 0).toUInt(),
                                                                map.value("packetSize", 1000).toUInt(),
                                                                map.value("trainLength", 48).toUInt(),
                                                                map.value("iterations", 1).toUInt(),
                                                                map.value("rateMin", 10485760).toUInt(),
                                                                map.value("rateMax", 262144000).toUInt(),
                                                                map.value("delay", 200000000).toUInt()));
}
