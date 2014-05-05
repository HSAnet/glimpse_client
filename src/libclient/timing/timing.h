#ifndef TIMING_H
#define TIMING_H

#include "../serializable.h"
#include "ntp.h"
#include <QDateTime>
#include <QSharedPointer>

class Timing;
typedef QSharedPointer<Timing> TimingPtr;

class CLIENT_API Timing : public Serializable
{
public:
    int timeLeft(const QDateTime& when = QDateTime::currentDateTime()) const;

    virtual ~Timing() {}

    virtual QString type() const = 0;
    virtual bool reset() = 0; // true = reset; false = stop execute
    virtual QDateTime nextRun() const = 0;

    static Ntp ntp;
};

#endif // TIMING_H
