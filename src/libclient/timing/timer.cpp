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
    , active(false)
    {
        qtimer.setSingleShot(true);
        qtimer.setTimerType(Qt::PreciseTimer);
        connect(&qtimer, SIGNAL(timeout()), this, SIGNAL(onTimeout()));
    }

    Timer *q;

    // Properties
    QTimer qtimer;
    TimingPtr timing;

    bool active;

    // Functions
    void setActive(const bool active);

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

void Timer::Private::onTimeout()
{
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
        d->setActive(true);

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
    d->setActive(false);
}

#include "timer.moc"
