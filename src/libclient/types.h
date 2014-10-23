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
 *
 * enumFromString removed for now, we don't use it and the old version would probably not work
 */
#define enumToString(className, enumName, value) \
    className::staticMetaObject.enumerator(className::staticMetaObject.indexOfEnumerator(#enumName)).key(value)


template<bool>
struct ToVariantImpl;

template <>
struct ToVariantImpl<0> {
    template <typename T>
    static T toType(const QVariant& value) {
        return qvariant_cast<T>(value);
    }
};

template <>
struct ToVariantImpl<1> {
    template <typename T>
    static T toType(const QVariant& value) {
        return T::fromVariant(value);
    }
};

template <typename T>
T variantToType(const QVariant& value)
{
    return ToVariantImpl<QTypeInfo<T>::isComplex>::template toType<T>(value);
}

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
        lst.append(variantToType<T>(entry));
    }

    return lst;
}

template <typename T>
QVariantList setToVariant(const QSet<T> &set)
{
    QVariantList lst;

    foreach (T entry, set)
    {
        lst.append(QVariant::fromValue(entry));
    }

    return lst;
}

template <typename T>
QSet<T> setFromVariant(const QVariant &variant)
{
    QSet<T> set;

    foreach (const QVariant &entry, variant.toList())
    {
        set.insert(variantToType<T>(entry));
    }

    return set;
}

/// Converts a QUuid to a QString. It removes the braces
/// and makes it readable by QML's implementation of
/// JSON.stringify()
QString uuidToString(const QUuid &uuid);

#endif // TYPES_H
