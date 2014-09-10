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
    CalendarTiming(const QDateTime &start, const QDateTime &end, const QList<quint8> &months,
                   const QList<quint8> &daysOfWeek, const QList<quint8> &daysOfMonth,
                   const QList<quint8> &hours, const QList<quint8> &minutes,
                   const QList<quint8> &seconds);
    ~CalendarTiming();

    // Storage
    static TimingPtr fromVariant(const QVariant &variant);

    // Getters
    QDateTime start() const;
    QDateTime end() const;
    QList<quint8> months() const;
    QList<quint8> daysOfWeek() const;
    QList<quint8> daysOfMonth() const;
    QList<quint8> hours() const;
    QList<quint8> minutes() const;
    QList<quint8> seconds() const;
    TimingRandomness randomness() const;

    // Timing interface
    QString type() const;
    bool reset();
    QDateTime nextRun() const;
    bool isValid() const;

    // Serializable interface
    QVariant toVariant() const;

protected:
    class Private;
    Private *d;
};

#endif // CALENDARTIMING_H
