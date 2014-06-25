#ifndef PERIODICTIMING_H
#define PERIODICTIMING_H

#include "timing.h"

class CLIENT_API PeriodicTiming : public Timing
{
public:
    PeriodicTiming(int interval,
                   const QDateTime &start = QDateTime(),
                   const QDateTime &end = QDateTime());
    ~PeriodicTiming();

    // Storage
    static TimingPtr fromVariant(const QVariant &variant);

    // Getters
    QDateTime start() const;
    QDateTime end() const;
    int interval() const;

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

#endif // PERIODICTIMING_H
