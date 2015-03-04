#include "timer.h"
#include "../log/logger.h"

#include <limits>
#include <QTimer>

LOGGER(Timer)

class Timer::Private : public QObject
{
    Q_OBJECT

public:
    Private(Timer *q)
    : q(q)
    , active(false)
    {
        qtimer.setSingleShot(true);
        qtimer.setTimerType(Qt::PreciseTimer);
        connect(&qtimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    }

    Timer *q;

    // Properties
    QTimer qtimer;
    TimingPtr timing;
    qint64 timeLeft;

    bool active;

    // Functions
    void setActive(const bool active);
    void start(qint64 ms);

public slots:
    void onTimeout();
};

void Timer::Private::setActive(const bool active)
{
    if (this->active != active)
    {
        this->active = active;
        emit q->activeChanged();
    }
}

void Timer::Private::start(qint64 ms)
{
    // If the time is bigger than maxInt start a timer with int
    int maxInt = std::numeric_limits<int>::max();

    if (ms > maxInt)
    {
        timeLeft = ms - maxInt;
        qtimer.start(maxInt);
    }
    else
    {
        timeLeft = 0;
        qtimer.start(ms);
    }
}

void Timer::Private::onTimeout()
{
    // If the timeout time is not reached yet (happens if the original time was bigger than maxInt
    if (timeLeft > 0)
    {
        start(timeLeft);
        return;
    }

    // Send the timeout signal
    emit q->timeout();

    // If we're still active, start the timer again
    if (active)
    {
        q->start();
    }
}

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

bool Timer::isActive() const
{
    return d->active;
}

void Timer::start()
{
    if (d->timing)
    {
        qint64 ms = d->timing->timeLeft();

        // In case timeleft is invalid, do not start the timer!
        if (ms == 0)
        {
            d->setActive(false);
            return;
        }

        d->setActive(true);

        d->start(ms);
    }
    else
    {
        LOG_ERROR(QString("Could not start timer, timing not set"));
    }
}

void Timer::stop()
{
    d->qtimer.stop();
    d->setActive(false);
}

#include "timer.moc"
