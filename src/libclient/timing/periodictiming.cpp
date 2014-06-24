#include "periodictiming.h"

Q_GLOBAL_STATIC(Ntp, ntp)

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

    return !nextRun().isNull();
}

QDateTime PeriodicTiming::nextRun() const
{
    QDateTime nextRun;
    const QDateTime now = QDateTime::currentDateTime().addSecs(ntp->offset());

    // Check if the start time is reached
    if (d->start > now)
    {
        return d->start;
    }

    // Calculate number of completed periods
    quint64 completedPeriods = d->start.msecsTo(now) / d->period;
    // Add the already executed periods to the start time + 1
    nextRun = d->start.addMSecs((completedPeriods + 1) * d->period);

    // Stop if we exceed the end time
    if (d->end.isValid() && d->end < nextRun)
    {
        return QDateTime();
    }

    return nextRun;
}

bool PeriodicTiming::isValid() const
{
    if (!d->start.isValid() || !d->end.isValid() || d->period <= 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

QVariant PeriodicTiming::toVariant() const
{
    QVariantMap hash;
    hash.insert("type", type());
    hash.insert("start", d->start);
    hash.insert("end", d->end);
    hash.insert("interval", d->period);
    return hash;
}

TimingPtr PeriodicTiming::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TimingPtr(new PeriodicTiming(hash.value("interval").toInt(),
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

int PeriodicTiming::interval() const
{
    return d->period;
}
