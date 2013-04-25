#ifndef TYPES_H
#define TYPES_H

#include <QMetaObject>
#include <QMetaEnum>

// UDP request types
enum RequestType
{
    UnknownRequest = 0,

    // Server requests
    ClientInfoRequest,

    // Client requests
    PeerResponse
};

enum Direction
{
    Direction_Any,
    Direction_Master,
    Direction_Slave
};

enum PlatformType
{
    Platform_Desktop,
    Platform_Mobile,
    Platform_Embedded
};

enum NetworkType
{
    Network_Mobile,
    Network_Cable,
    Network_DSL,
    Network_Satellite,
    Network_Ethernet
};

#define enumToString(className, enumName, value) \
    className::staticMetaObject.enumerator(className::staticMetaObject.indexOfEnumerator(enumName)).valueToKey(value)

#endif // TYPES_H
