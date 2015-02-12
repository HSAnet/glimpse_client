#ifndef TIMER_H
#define TIMER_H

#include "timing.h"

#include <QObject>

class CLIENT_API Timer : public QObject
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

    bool isActive() const;

signals:
    void timeout();
    void timingChanged();
    void activeChanged();

public slots:
    void start();
    void stop();

protected:
    class Private;
    Private *d;
};

#endif // TIMER_H
