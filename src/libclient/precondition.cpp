#include "precondition.h"
#include "network/networkmanager.h"
#include "deviceinfo.h"
#include "client.h"
#include "log/logger.h"

#include <QSharedData>

LOGGER(Precondition);

class PreconditionData : public QSharedData
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
: d(new PreconditionData)
{
    d->onWireless = true;
    d->onWire = true;
    d->onCellular = true;
    d->minCharge = 0;
    d->locLat = 0.0;
    d->locLong = 0.0;
    d->locRadius = 0;
}

Precondition::Precondition(const Precondition &rhs)
: d(rhs.d)
{
}

Precondition::Precondition(bool onWireless, bool onWire, bool onCellular, quint16 minCharge, double locLat, double locLong, double locRadius)
: d(new PreconditionData)
{
    d->onWireless = onWireless;
    d->onWire = onWire;
    d->onCellular = onCellular;
    d->minCharge = minCharge;
    d->locLat = locLat;
    d->locLong = locLong;
    d->locRadius = locRadius;
}

Precondition::~Precondition()
{
}

Precondition &Precondition::operator=(const Precondition &rhs)
{
    if (this != &rhs)
    {
        d.operator=(rhs.d);
    }

    return *this;
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
    const NetworkManager *nm = Client::instance()->networkManager();

    // Networking
    QNetworkInfo::NetworkMode mode = nm->connectionMode();

    if (!d->onWireless && mode == QNetworkInfo::WlanMode)
    {
        LOG_DEBUG("on wireless connection");
        return false;
    }

    if (!d->onWire && mode == QNetworkInfo::EthernetMode)
    {
        LOG_DEBUG("on ethernet connection");
        return false;
    }

    if (!d->onCellular && nm->onMobileConnection())
    {
        LOG_DEBUG("on mobile connection");
        return false;
    }

    // Battery level
    qint8 batteryLevel = DeviceInfo().batteryLevel();
    if (batteryLevel <= 0 && batteryLevel < d->minCharge)
    {
        LOG_DEBUG("batteryLevel too low");
        return false;
    }

    // Location TODO



    return true;
}
