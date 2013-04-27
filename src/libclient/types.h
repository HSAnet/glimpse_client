#ifndef TYPES_H
#define TYPES_H

#include <QMetaObject>
#include <QMetaEnum>
#include <QHostAddress>
#include <QList>

// UDP request types
enum RequestType
{
    UnknownRequest = 0,

    // Server requests
    ClientInfoRequest,
    PeerRequest,

    // Client requests
    PeerResponse = 1024
};

#define enumToString(className, enumName, value) \
    className::staticMetaObject.enumerator(className::staticMetaObject.indexOfEnumerator(enumName)).valueToKey(value)

#endif // TYPES_H
