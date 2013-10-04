#ifndef TIMING_H
#define TIMING_H

#include "export.h"

#include <QVariant>
#include <QDateTime>
#include <QSharedPointer>


class Timing;
typedef QSharedPointer<Timing> TimingPtr;


class CLIENT_API Serializable
{
public:
    virtual ~Serializable() {}
    virtual QVariant toVariant() const = 0;
};


class CLIENT_API Timing : public Serializable
{
public:
    int timeLeft(const QDateTime& when = QDateTime::currentDateTime()) const;

    virtual ~Timing() {}

    virtual QString type() const;
    virtual bool reset() = 0; // true = reset; false = stop execute
    virtual QDateTime nextRun() const = 0;
};

#endif // TIMING_H
