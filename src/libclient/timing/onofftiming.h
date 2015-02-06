#ifndef ONOFFTIMING_H
#define ONOFFTIMING_H

#include "timing.h"

class CLIENT_API OnOffTiming : public Timing
{
public:
    OnOffTiming(const QDateTime &dateTime);
    ~OnOffTiming();

    // Storage
    static TimingPtr fromVariant(const QVariant &variant);

    // Getters
    QDateTime dateTime() const;

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

#endif // ONOFFTIMING_H
