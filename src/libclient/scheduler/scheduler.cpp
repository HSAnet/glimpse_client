#include "scheduler.h"
#include "../task/taskexecutor.h"
#include "../log/logger.h"

#include <QDir>
#include <QTimer>
#include <QCoreApplication>
#include <QDebug>
#include <QPointer>
#include <QSet>

LOGGER(Scheduler);

class Scheduler::Private : public QObject
{
    Q_OBJECT

public:
    Private(Scheduler* q)
    : q(q)
    , path(qApp->applicationDirPath())
    {
        timer.setSingleShot(true);
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

    Scheduler* q;

    // Properties
    QDir path;
    QTimer timer;

    TestDefinitionList tests;
    QSet<QUuid> testIds;

    QPointer<TaskExecutor> executor;

    // Functions
    void updateTimer();
    int enqueue(const TestDefinitionPtr &testDefinition);

public slots:
    void timeout();
};

void Scheduler::Private::updateTimer()
{
    if (tests.isEmpty())
    {
        timer.stop();
        LOG_DEBUG("Scheduling timer stopped");
    }
    else
    {
        const TestDefinitionPtr td = tests.at(0);
        int ms = td->timing()->timeLeft();
        if ( ms > 0 )
        {
            timer.start( ms );
        }
        else
        {
            // If we would call timeout() directly, the testAdded() signal
            // would be emitted after execution.
            timer.start(1);
        }

        LOG_DEBUG(QString("Scheduling timer executes %1 in %2 ms").arg(td->name()).arg(ms));
    }
}

int Scheduler::Private::enqueue(const TestDefinitionPtr& testDefinition)
{
    if ( testIds.contains(testDefinition->id()) )
    {
        return -1;
    }

    bool wasEmpty = tests.isEmpty();

    int timeLeft = testDefinition->timing()->timeLeft();

    for (int i=0; i < tests.size(); ++i)
    {
        const TestDefinitionPtr& td = tests.at(i);
        if (timeLeft < td->timing()->timeLeft())
        {
            tests.insert(i, testDefinition);
            testIds.insert(testDefinition->id());

            if ( wasEmpty || i == 0 )
            {
                updateTimer();
            }

            return i;
        }
    }

    tests.append(testDefinition);

    if ( wasEmpty )
    {
        updateTimer();
    }

    return tests.size()-1;
}

void Scheduler::Private::timeout()
{
    for (int i=0; i < tests.size(); ++i)
    {
        TestDefinitionPtr td = tests.at(i);

        // We assume they are already sorted - WRONG, SO WRONG!
        if ( td->timing()->timeLeft() > 0 )
        {
            continue;
        }

        q->execute(td);

        tests.removeAt(i);

        if (!td->timing()->reset())
        {
            emit q->testRemoved(td, i);
            testIds.remove(td->id());

            if (tests.isEmpty())
            {
                break;
            }
            else
            {
                --i;
            }
        }
        else
        {
            int pos = enqueue(td);
            if (pos != i)
            {
                LOG_DEBUG(QString("%1 moved from %2 to %3").arg(td->name()).arg(i).arg(pos));
                emit q->testMoved(td, i, pos);
            }
        }
    }

    updateTimer();
}

Scheduler::Scheduler()
: d(new Private(this))
{
}

Scheduler::~Scheduler()
{
    delete d;
}

void Scheduler::setExecutor(TaskExecutor *executor)
{
    d->executor = executor;
}

TaskExecutor *Scheduler::executor() const
{
    return d->executor;
}

TestDefinitionList Scheduler::tests() const
{
    return d->tests;
}

void Scheduler::enqueue(const TestDefinitionPtr &testDefinition)
{
    int pos = d->enqueue(testDefinition);
    emit testAdded(testDefinition, pos);
}

void Scheduler::dequeue()
{
    // Whatever
}

void Scheduler::execute(const TestDefinitionPtr &testDefinition)
{
    d->executor->execute(testDefinition);
}

#include "scheduler.moc"
