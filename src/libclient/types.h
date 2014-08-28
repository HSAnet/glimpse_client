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

namespace ping
{
    enum PingType
    {
        Unknown,
        System,
        Udp,
        Tcp
    };
}

/*
 * The macros enum{To,From}String only work for classes which have a
 * QMetaObject variable named 'staticMetaObject'. The Qt macro 'Q_OBJECT' takes
 * care of this.
 */
#define enumToString(className, enumName, value) \
    className::staticMetaObject.enumerator(className::staticMetaObject.indexOfEnumerator("enumName")).valueToKey(value)

#define enumFromString(className, enumName, value) \
    static_cast<className::enumName>(className::staticMetaObject.enumerator( \
    className::staticMetaObject.indexOfEnumerator("enumName")).keyToValue(value))


inline QString pingTypeToString(const ping::PingType &type)
{
    if (type == ping::System)
    {
        return "System";
    }
    else if (type == ping::Udp)
    {
        return "Udp";
    }
    else if (type == ping::Tcp)
    {
        return "Tcp";
    }

    return "";
}

inline ping::PingType pingTypeFromString(const QString &type)
{
    if (type == "System")
    {
        return ping::System;
    }
    else if (type == "Udp")
    {
        return ping::Udp;
    }
    else if (type == "Tcp")
    {
        return ping::Tcp;
    }

    return ping::Unknown;
}

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
