#include "packettrainsdefinition.h"


PacketTrainsDefinition::PacketTrainsDefinition(QString host, quint16 port, quint16 packetSize, quint16 trainLength, quint8 iterations)
    : host(host)
    , port(port)
    , packetSize(packetSize)
    , trainLength(trainLength)
    , iterations(iterations)
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
    return map;
}

PacketTrainsDefinitionPtr PacketTrainsDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return PacketTrainsDefinitionPtr(new PacketTrainsDefinition(map.value("host").toString(),
                                                                map.value("port").toUInt(),
                                                                map.value("initialDataSize").toUInt(),
                                                                map.value("trainLength").toUInt(),
                                                                map.value("iterations").toUInt()));
}
