#include "periodictiming.h"

class PeriodicTiming::Private
{
public:
    QDateTime start;
    QDateTime end;
    int interval;

    QDateTime lastExecute;
};

PeriodicTiming::PeriodicTiming(int interval, const QDateTime &start, const QDateTime &end)
: d(new Private)
{
    d->interval = interval;
    d->start = start;
    d->end = end;

    reset();
}

PeriodicTiming::~PeriodicTiming()
{
    delete d;
}

QString PeriodicTiming::type() const
{
    return "periodic";
}

bool PeriodicTiming::reset()
{
    d->lastExecute = QDateTime::currentDateTime();
    return true;
}

QDateTime PeriodicTiming::nextRun() const
{
    return d->lastExecute.addMSecs(d->interval);
}

QVariant PeriodicTiming::toVariant() const
{
    QVariantMap hash;
    hash.insert("type", type());
    hash.insert("start", d->start);
    hash.insert("end", d->end);
    hash.insert("interval", d->interval);
    return hash;
}

TimingPtr PeriodicTiming::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TimingPtr(new PeriodicTiming(hash.value("interval").toInt(),
                                        hash.value("start").toDateTime(),
                                        hash.value("end").toDateTime()));
}
