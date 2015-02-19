#ifndef CALENDARTIMING_H
#define CALENDARTIMING_H

#include "timing.h"

class CLIENT_API CalendarTiming : public Timing
{
public:
    CalendarTiming(const QDateTime &start, const QDateTime &end, const QList<int> &months,
                   const QList<int> &daysOfWeek, const QList<int> &daysOfMonth,
                   const QList<int> &hours, const QList<int> &minutes,
                   const QList<int> &seconds);
    ~CalendarTiming();

    // Storage
    static TimingPtr fromVariant(const QVariant &variant);
    static const QList<int> AllMonths;
    static const QList<int> AllDaysOfWeek;
    static const QList<int> AllDaysOfMonth;
    static const QList<int> AllHours;
    static const QList<int> AllMinutes;
    static const QList<int> AllSeconds;

    // Getters
    QDateTime start() const;
    QDateTime end() const;
    QList<int> months() const;
    QList<int> daysOfWeek() const;
    QList<int> daysOfMonth() const;
    QList<int> hours() const;
    QList<int> minutes() const;
    QList<int> seconds() const;

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
