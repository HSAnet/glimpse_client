#ifndef TYPES_H
#define TYPES_H

#include <QMetaObject>
#include <QMetaEnum>
#include <QHostAddress>
#include <QList>
#include <QUuid>

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

#define enumFromString(className, enumName, value) \
    className::staticMetaObject.enumerator(className::staticMetaObject.indexOfEnumerator(enumName)).keyToValue(value)

template <typename T>
QVariantList ptrListToVariant(const QList<T> &list)
{
    QVariantList lst;

    foreach (T entry, list)
    {
        if (!entry.isNull())
        {
            lst.append(entry->toVariant());
        }
    }

    return lst;
}

template <typename T>
QList<QSharedPointer<T> > ptrListFromVariant(const QVariant &variant)
{
    QList<QSharedPointer<T> > lst;

    foreach (const QVariant &entry, variant.toList())
    {
        lst.append(T::fromVariant(entry));
    }

    return lst;
}

inline QVariant qVariantFromValue(const QHostAddress &address)
{
    return address.toString();
}

template <typename T>
QVariantList listToVariant(const QList<T> &list)
{
    QVariantList lst;

    foreach (T entry, list)
    {
        lst.append(QVariant::fromValue(entry));
    }

    return lst;
}

template <typename T>
QList<T> listFromVariant(const QVariant &variant)
{
    QList<T> lst;

    foreach (const QVariant &entry, variant.toList())
    {
        lst.append(T::fromVariant(entry));
    }

    return lst;
}

/// Converts a QUuid to a QString. It removes the braces
/// and makes it readable by QML's implementation of
/// JSON.stringify()
QString uuidToString(const QUuid &uuid);

#endif // TYPES_H
