#include "precondition.h"
#include "network/networkmanager.h"
#include "deviceinfo.h"

class Precondition::Private
{
public:
    bool onWireless;
    bool onWire;
    bool onCellular;
    quint16 minCharge;
    double locLat;
    double locLong;
    quint32 locRadius;
};

Precondition::Precondition()
: d(new Private)
{
    d->onWireless = true;
    d->onWire = true;
    d->onCellular = true;
    d->minCharge = 0;
    d->locLat = 0.0;
    d->locLong = 0.0;
    d->locRadius = 0;
}

Precondition::Precondition(bool onWireless, bool onWire, bool onCellular, quint16 minCharge, double locLat, double locLong, double locRadius)
: d(new Private)
{
    d->onWireless = onWireless;
    d->onWire = onWire;
    d->onCellular = onCellular;
    d->minCharge = minCharge;
    d->locLat = locLat;
    d->locLong = locLong;
    d->locRadius = locRadius;
}

Precondition Precondition::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return Precondition(hash.value("on_wireless").toBool(),
                        hash.value("on_wire").toBool(),
                        hash.value("on_cellular").toBool(),
                        hash.value("min_charge").toUInt(),
                        hash.value("loc_lat").toDouble(),
                        hash.value("loc_long").toDouble(),
                        hash.value("loc_radius").toUInt());
}

QVariant Precondition::toVariant() const
{
    QVariantMap hash;

    hash.insert("on_wireless", d->onWireless);
    hash.insert("on_wire", d->onWire);
    hash.insert("on_cellular", d->onCellular);
    hash.insert("min_charge", d->onCellular);
    hash.insert("loc_lat", d->onCellular);
    hash.insert("loc_long", d->onCellular);
    hash.insert("loc_radius", d->locRadius);

    return hash;
}

bool Precondition::check()
{
    NetworkManager nm;
    // Networking
    QList<QNetworkConfiguration::BearerType> typeList = nm.connectionType();

    if (!d->onWireless && typeList.contains(QNetworkConfiguration::BearerWLAN))
    {
        return false;
    }

    if (!d->onWire && typeList.contains(QNetworkConfiguration::BearerEthernet))
    {
        return false;
    }

    if (!d->onCellular && nm.onMobileConnection())
    {
        return false;
    }

    // Battery level
    if (DeviceInfo::batteryLevel() < d->minCharge)
    {
        return false;
    }

    // Location TODO


    return true;
}
