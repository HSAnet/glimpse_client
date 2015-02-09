#ifndef TIMER_H
#define TIMER_H

#include "timing.h"

#include <QObject>

class Timer : public QObject
{
    Q_OBJECT
public:
    explicit Timer(QObject *parent = 0);
    explicit Timer(const TimingPtr &timing, QObject *parent = 0);
    ~Timer();

    void setTimerType(Qt::TimerType atype);
    Qt::TimerType timerType() const;

    void setTiming(const TimingPtr &timing);
    TimingPtr timing() const;

signals:
    void timeout();
    void timingChanged();

public slots:
    void start();
    void stop();

protected:
    class Private;
    Private *d;
};

#endif // TIMER_H
