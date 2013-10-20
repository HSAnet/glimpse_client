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
    RegisteredClientResponse,

    // Client requests
    PeerResponse = 1024
};

#define enumToString(className, enumName, value) \
    className::staticMetaObject.enumerator(className::staticMetaObject.indexOfEnumerator(enumName)).valueToKey(value)

template <typename T>
QVariant ptrListToVariant(const QList<T>& list) {
    QVariantList lst;
    foreach(T entry, list) {
        if (!entry.isNull())
            lst.append(entry->toVariant());
    }
    return lst;
}

template <typename T>
QList<QSharedPointer<T> > ptrListFromVariant(const QVariant& variant) {
    QList<QSharedPointer<T> > lst;
    foreach(const QVariant& entry, variant.toList())
        lst.append(T::fromVariant(entry));
    return lst;
}

#endif // TYPES_H
