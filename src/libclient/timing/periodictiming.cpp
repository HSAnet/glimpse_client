#include "periodictiming.h"

class PeriodicTiming::Private
{
public:
    QDateTime start;
    QDateTime end;
    int period;

    QDateTime lastExecute;
};

PeriodicTiming::PeriodicTiming(int period, const QDateTime &start, const QDateTime &end)
: d(new Private)
{
    d->period = period;
    d->start = start;
    d->end = end;

    //reset();
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

    if (nextRun() > d->end)
        return false;

    return true;
}

QDateTime PeriodicTiming::nextRun() const
{
    if ( d->lastExecute.isNull() )
        return QDateTime::currentDateTime();
    else
        return d->lastExecute.addMSecs(d->period);
}

QVariant PeriodicTiming::toVariant() const
{
    QVariantMap hash;
    hash.insert("type", type());
    hash.insert("start", d->start);
    hash.insert("end", d->end);
    hash.insert("period", d->period);
    return hash;
}

TimingPtr PeriodicTiming::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TimingPtr(new PeriodicTiming(hash.value("period").toInt(),
                                        hash.value("start").toDateTime(),
                                        hash.value("end").toDateTime()));
}

QDateTime PeriodicTiming::start() const
{
    return d->start;
}

QDateTime PeriodicTiming::end() const
{
    return d->end;
}

int PeriodicTiming::period() const
{
    return d->period;
}
