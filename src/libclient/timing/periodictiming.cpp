#include "periodictiming.h"
#include "client.h"
#include "../controller/ntpcontroller.h"

class PeriodicTiming::Private
{
public:
    QDateTime start;
    QDateTime end;
    int period;
    int randomSpread;
    int randomDelay;
};

PeriodicTiming::PeriodicTiming(int period, const QDateTime &start, const QDateTime &end, int randomSpread)
: d(new Private)
{
    d->period = period;
    d->start = start;
    d->end = end;
    d->randomSpread = randomSpread;

    if (d->randomSpread)
    {
        qsrand(QDateTime::currentMSecsSinceEpoch());
        d->randomDelay = qrand() % (d->randomSpread);
    }
    else
    {
        d->randomDelay = 0;
    }
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
    m_lastExecution = Client::instance()->ntpController()->currentDateTime();

    return nextRun().isValid();
}

QDateTime PeriodicTiming::nextRun(const QDateTime &tzero) const
{
    QDateTime nextRun;
    QDateTime now;

    if (tzero.isValid())
    {
        now = tzero;
    }
    else
    {
        now = Client::instance()->ntpController()->currentDateTime();
    }

    // Check if end time is reached
    if (d->end.isValid() && d->end < now)
    {
        return QDateTime();
    }

    // Check if the start time is reached
    if (d->start.isValid() && d->start > now)
    {
        return d->start;
    }

    if (!d->start.isValid())
    {
        d->start = now;
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

    // add random delay
    return nextRun.addMSecs(d->randomDelay);
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
    hash.insert("start", d->start);
    hash.insert("end", d->end);
    hash.insert("interval", d->period);
    hash.insert("randomSpread", d->randomSpread);

    QVariantMap resultMap;
    resultMap.insert(type(), hash);
    return resultMap;
}

TimingPtr PeriodicTiming::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TimingPtr(new PeriodicTiming(hash.value("interval").toInt(),
                                        hash.value("start", QDateTime()).toDateTime(),
                                        hash.value("end", QDateTime()).toDateTime(),
                                        hash.value("randomSpread", 0).toInt()));
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

int PeriodicTiming::randomSpread() const
{
    return d->randomSpread;
}
