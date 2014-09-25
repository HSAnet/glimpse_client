#include "calendartiming.h"
#include "types.h"
#include <algorithm>

Q_GLOBAL_STATIC(Ntp, ntp)

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

    QVariantMap resultMap;
    resultMap.insert("calendar", map);
    return resultMap;
}


class CalendarTiming::Private
{
public:
    QDateTime start;
    QDateTime end;
    QList<quint8> months; // default: 1-12
    QList<quint8> daysOfWeek; // default: 1-7, 1 = Monday, 7 = Sunday
    QList<quint8> daysOfMonth; // default 1-31
    QList<quint8> hours; // default: 0-23
    QList<quint8> minutes; // default: 0-59
    QList<quint8> seconds; // default: 0-59
    TimingRandomness randomness;

    QTime findTime(QTime &referenceTime);
};

QTime CalendarTiming::Private::findTime(QTime &referenceTime)
{
    QListIterator<quint8> hourIter(hours);
    QListIterator<quint8> minuteIter(minutes);
    QListIterator<quint8> secondIter(seconds);

    int hour;
    int minute;
    int second;

    while (hourIter.hasNext()) // hours
    {
        hour = hourIter.next();
        if (hour >= referenceTime.hour())
        {
            // hour found
            while (minuteIter.hasNext()) // minutes
            {
                minute = minuteIter.next();
                if (minute >= referenceTime.minute())
                {
                    // minute found
                    while (secondIter.hasNext()) // seconds
                    {
                        second = secondIter.next();
                        if (second >= referenceTime.second())
                        {
                            // finally, second found
                            return QTime(hour, minute, second);
                        }

                        if (!secondIter.hasNext())
                        {
                            secondIter.toFront();
                            referenceTime.setHMS(referenceTime.hour(), referenceTime.minute(), 0);
                            break;
                        }
                    }
                }

                if (!minuteIter.hasNext())
                {
                    minuteIter.toFront();
                    referenceTime.setHMS(referenceTime.hour(), 0, 0);
                    break;
                }
            }
        }
    }

    referenceTime.setHMS(0, 0, 0);
    return QTime();
}

CalendarTiming::CalendarTiming()
: d(new Private)
{
    d->start = QDateTime::currentDateTime();
    d->end = QDateTime();
    d->months = QList<quint8>()<<1<<2<<3<<4<<5<<6<<7<<8<<9<<10<<11<<12;
    d->daysOfWeek = QList<quint8>()<<1<<2<<3<<4<<5<<6<<7;

    for (int i = 1; i < 32; i++)
    {
        d->daysOfMonth<<i;
    }

    for (int i = 0; i < 24; i++)
    {
        d->hours<<i;
    }

    for (int i = 0; i < 60; i++)
    {
        d->minutes<<i;
        d->seconds<<i;
    }

    // make sure the lists are sorted (should already be the case)
    std::sort(d->months.begin(), d->months.end());
    std::sort(d->daysOfWeek.begin(), d->daysOfWeek.end());
    std::sort(d->daysOfMonth.begin(), d->daysOfMonth.end());
    std::sort(d->hours.begin(), d->hours.end());
    std::sort(d->minutes.begin(), d->minutes.end());
    std::sort(d->seconds.begin(), d->seconds.end());
}

CalendarTiming::CalendarTiming(const QDateTime &start, const QDateTime &end, const QList<quint8> &months,
                               const QList<quint8> &daysOfWeek, const QList<quint8> &daysOfMonth,
                               const QList<quint8> &hours, const QList<quint8> &minutes,
                               const QList<quint8> &seconds)
: d(new Private)
{
    d->start = start;
    d->end = end;
    d->months = months;
    d->daysOfWeek = daysOfWeek;
    d->daysOfMonth = daysOfMonth;
    d->hours = hours;
    d->minutes = minutes;
    d->seconds = seconds;

    // make sure the lists are sorted
    std::sort(d->months.begin(), d->months.end());
    std::sort(d->daysOfWeek.begin(), d->daysOfWeek.end());
    std::sort(d->daysOfMonth.begin(), d->daysOfMonth.end());
    std::sort(d->hours.begin(), d->hours.end());
    std::sort(d->minutes.begin(), d->minutes.end());
    std::sort(d->seconds.begin(), d->seconds.end());
}

CalendarTiming::~CalendarTiming()
{
    delete d;
}

bool CalendarTiming::reset()
{
    return true;
}

QDateTime CalendarTiming::nextRun() const
{
    QDateTime nextRun;
    QDateTime now = QDateTime::currentDateTime().addSecs(ntp->offset());

    QDate date = now.date();
    QTime time = now.time();
    bool found = false;

    QListIterator<quint8> monthIter(d->months);
    QListIterator<quint8> dayIter(d->daysOfMonth);
    QDate nextRunDate;
    QTime nextRunTime;

    // Check if the start time is reached
    if (d->start > now)
    {
        date.setDate(d->start.date().year(), d->start.date().month(), d->start.date().day());
        time.setHMS(d->start.time().hour(), d->start.time().minute(), d->start.time().second());
        now.setDate(date);
        now.setTime(time);
    }

    int year = date.year();
    int month = date.month();
    int day = date.day();

    while (!found) // years
    {
        while (monthIter.hasNext() && !found) // months
        {
            month = monthIter.next();
            if (QDate(year, month, 1) >= QDate(date.year(), date.month(), 1))
            {
                // change to first element
                dayIter.toFront();

                while (dayIter.hasNext() && !found) // days
                {
                    day = dayIter.next();
                    if (QDate(year, month, day) >= date)
                    {
                        // potential day found, check if day of week is okay
                        if (d->daysOfWeek.contains(QDate(year, month, day).dayOfWeek()))
                        {
                            // check if we have a matching time for that day
                            if ((nextRunDate = QDate(year, month, day)) != now.date())
                            {
                                // if the next run date is not today we can take the first items
                                // as this is the earliest allowed time on that day
                                nextRunTime = QTime(*d->hours.constBegin(), *d->minutes.constBegin(), *d->seconds.constBegin());
                                nextRun = QDateTime(nextRunDate, nextRunTime);
                                found = true;
                            }
                            else if ((nextRunTime = d->findTime(time)).isValid())
                            {
                                nextRun = QDateTime(nextRunDate, nextRunTime);
                                found = true;
                            }
                        }
                    }

                    // check if end of set reached (= end of month)
                    if (!dayIter.hasNext())
                    {
                        date.setDate(date.year(), date.month(), 1);
                        break;
                    }
                }
            }

            // end of set reached, the first element is the right
            if (!monthIter.hasNext())
            {
                monthIter.toFront();
                date.setDate(++year, 1, 1);
                break;
            }
        }

        // for saftey reasons
        if (year > now.date().year() + 2)
        {
            // no next run found
            return QDateTime();
        }
    }

    // Stop if we exceed the end time
    if (d->end.isValid() && d->end < nextRun)
    {
        return QDateTime();
    }

    return nextRun;
}

bool CalendarTiming::isValid() const
{
    return false; // not implemented yet
}

QVariant CalendarTiming::toVariant() const
{
    QVariantMap hash;
    hash.insert("start", d->start);
    hash.insert("end", d->end);
    hash.insert("months", listToVariant(d->months));
    hash.insert("days_of_week", listToVariant(d->daysOfWeek));
    hash.insert("days_of_month", listToVariant(d->daysOfMonth));
    hash.insert("hours", listToVariant(d->hours));
    hash.insert("minutes", listToVariant(d->minutes));
    hash.insert("seconds", listToVariant(d->seconds));
    //hash.insert("randomness", d->randomness.toVariant());

    QVariantMap resultMap;
    resultMap.insert(type(), hash);
    return resultMap;
}

TimingPtr CalendarTiming::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TimingPtr(new CalendarTiming(hash.value("start").toDateTime(),
                                        hash.value("end").toDateTime(),
                                        listFromVariant<quint8>(hash.value("months")),
                                        listFromVariant<quint8>(hash.value("days_of_week")),
                                        listFromVariant<quint8>(hash.value("days_of_month")),
                                        listFromVariant<quint8>(hash.value("hours")),
                                        listFromVariant<quint8>(hash.value("seconds")),
                                        listFromVariant<quint8>(hash.value("minutes"))));
}

QDateTime CalendarTiming::start() const
{
    return d->start;
}

QDateTime CalendarTiming::end() const
{
    return d->end;
}

QList<quint8> CalendarTiming::months() const
{
    return d->months;
}

QList<quint8> CalendarTiming::daysOfWeek() const
{
    return d->daysOfWeek;
}

QList<quint8> CalendarTiming::daysOfMonth() const
{
    return d->daysOfMonth;
}

QList<quint8> CalendarTiming::hours() const
{
    return d->hours;
}

QList<quint8> CalendarTiming::minutes() const
{
    return d->minutes;
}

QList<quint8> CalendarTiming::seconds() const
{
    return d->seconds;
}

TimingRandomness CalendarTiming::randomness() const
{
    return d->randomness;
}

QString CalendarTiming::type() const
{
    return "calendar";
}
