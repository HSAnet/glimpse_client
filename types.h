#ifndef TYPES_H
#define TYPES_H

#include <QMetaObject>
#include <QMetaEnum>

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

/*
QMetaEnum findMetaEnum(const QLatin1String& name, const QMetaObject* meta) {
    int cnt = meta->enumeratorCount();
    for(int i=0; i < cnt; ++i) {
        QMetaEnum e = meta->enumerator(i);
        if (qstrcmp(e.name(), name.latin1()) == 0)
            return e;
    }

    return QMetaEnum();
}
*/

#endif // TYPES_H
