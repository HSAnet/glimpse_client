#include "calendartiming.h"

class TimingRandomness::Private
{
public:
    QString otherDistribution; // Filled with other values
    TimingRandomness::DistributionType distribution;
    int upperCut;
    int lowerCut;
    int spread;
};

TimingRandomness::TimingRandomness()
: d(new Private)
{
}

TimingRandomness::~TimingRandomness()
{
    delete d;
}

QString TimingRandomness::otherDistribution() const
{
    return d->otherDistribution;
}

TimingRandomness::DistributionType TimingRandomness::distribution() const
{
    return d->distribution;
}

int TimingRandomness::upperCut() const
{
    return d->upperCut;
}

int TimingRandomness::lowerCut() const
{
    return d->lowerCut;
}

int TimingRandomness::spread() const
{
    return d->spread;
}

QVariant TimingRandomness::toVariant() const
{
    QVariantMap map;
    map.insert("distribution", ""); // TODO: Fill
    map.insert("upperCut", d->upperCut);
    map.insert("lowerCut", d->lowerCut);
    map.insert("spread", d->spread);
    return map;
}


class CalendarTiming::Private
{
public:
    TimingRandomness randomness;
};

CalendarTiming::CalendarTiming()
: d(new Private)
{
}

CalendarTiming::~CalendarTiming()
{
    delete d;
}

bool CalendarTiming::reset() {
    return true;
}

QDateTime CalendarTiming::nextRun() const {
    return QDateTime(); // never
}

QVariant CalendarTiming::toVariant() const {
    QVariantMap hash;
    hash.insert("type", type());
    hash.insert("randomness", d->randomness.toVariant());
    return hash;
}

TimingPtr CalendarTiming::fromVariant(const QVariant &variant) {
    Q_UNUSED(variant);
    return TimingPtr(new CalendarTiming);
}

TimingRandomness CalendarTiming::randomness() const
{
    return d->randomness;
}

QString CalendarTiming::type() const
{
    return "calendar";
}
