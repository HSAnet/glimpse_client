#ifndef CALENDARTIMING_H
#define CALENDARTIMING_H

#include "timing.h"

class CLIENT_API TimingRandomness : public Serializable
{
public:
    TimingRandomness();
    ~TimingRandomness();

    enum DistributionType
    {
        Other,
        Poisson,
        Normal,
        Uniform
    };

    QString otherDistribution() const; // Filled with other values
    DistributionType distribution() const;
    int upperCut() const;
    int lowerCut() const;
    int spread() const;

    // Serializable interface
    QVariant toVariant() const;

protected:
    class Private;
    Private *d;
};


class CLIENT_API CalendarTiming : public Timing
{
public:
    CalendarTiming();
    CalendarTiming(const QDateTime &start, const QDateTime &end, const QList<int> &months,
                   const QList<int> &daysOfWeek, const QList<int> &daysOfMonth,
                   const QList<int> &hours, const QList<int> &minutes,
                   const QList<int> &seconds);
    ~CalendarTiming();

    // Storage
    static TimingPtr fromVariant(const QVariant &variant);

    // Getters
    QDateTime start() const;
    QDateTime end() const;
    QList<int> months() const;
    QList<int> daysOfWeek() const;
    QList<int> daysOfMonth() const;
    QList<int> hours() const;
    QList<int> minutes() const;
    QList<int> seconds() const;
    TimingRandomness randomness() const;

    // Timing interface
    QString type() const;
    bool reset();
    QDateTime nextRun(const QDateTime &tzero = QDateTime()) const;
    bool isValid() const;

    // Serializable interface
    QVariant toVariant() const;

protected:
    class Private;
    Private *d;
};

#endif // CALENDARTIMING_H
