#include "timer.h"
#include "../log/logger.h"

#include <QTimer>

LOGGER(Timer)

class Timer::Private : public QObject
{
    Q_OBJECT

public:
    Private(Timer *q)
    : q(q)
    {
        qtimer.setSingleShot(true);
        qtimer.setTimerType(Qt::PreciseTimer);
        connect(&qtimer, SIGNAL(timeout()), q, SLOT(start()));
        connect(&qtimer, SIGNAL(timeout()), q, SIGNAL(timeout()));
    }

    Timer *q;
    QTimer qtimer;
    TimingPtr timing;
};

Timer::Timer(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

Timer::Timer(const TimingPtr &timing, QObject *parent)
: QObject(parent)
, d(new Private(this))
{
    setTiming(timing);
}

Timer::~Timer()
{
    delete d;
}

void Timer::setTimerType(Qt::TimerType atype)
{
    d->qtimer.setTimerType(atype);
}

Qt::TimerType Timer::timerType() const
{
    return d->qtimer.timerType();
}

void Timer::setTiming(const TimingPtr &timing)
{
    if (timing->type() == "immediate")
    {
        LOG_ERROR(QString("Immediate timing not allowed for timer"));
        return;
    }

    if (d->timing != timing)
    {
        d->timing = timing;
        emit timingChanged();
    }
}

TimingPtr Timer::timing() const
{
    return d->timing;
}

void Timer::start()
{
    if (d->timing)
    {
        d->qtimer.start(d->timing->timeLeft());
    }
    else
    {
        LOG_ERROR(QString("Could not start timer, timing not set"));
    }
}

void Timer::stop()
{
    d->qtimer.stop();
}

#include "timer.moc"
