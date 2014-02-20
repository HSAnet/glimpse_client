#include "scheduler.h"
#include "../task/taskexecutor.h"
#include "../log/logger.h"
#include "../timing/ondemandtiming.h"

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
        timer.setTimerType(Qt::PreciseTimer);
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

    Scheduler* q;

    // Properties
    QDir path;
    QTimer timer;

    TestDefinitionList tests;
    TestDefinitionList onDemandTests;
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
            LOG_DEBUG(QString("Scheduling timer executes %1 in %2 ms").arg(td->name()).arg(ms));
            timer.start( ms );
        }
        else
        {
            // If we would call timeout() directly, the testAdded() signal
            // would be emitted after execution.
            LOG_DEBUG(QString("Scheduling timer executes %1 now").arg(td->name()));
            timer.start(1);
        }

    }
}

int Scheduler::Private::enqueue(const TestDefinitionPtr& testDefinition)
{
    if ( testIds.contains(testDefinition->id()) )
    {
        return -1;
    }

    int timeLeft = testDefinition->timing()->timeLeft();

    for (int i=0; i < tests.size(); i++)
    {
        const TestDefinitionPtr& td = tests.at(i);
        if (timeLeft < td->timing()->timeLeft())
        {
            tests.insert(i, testDefinition);
            testIds.insert(testDefinition->id());

            // update the timer if this is the new first element
            if (i == 0)
            {
                updateTimer();
            }

            return i;
        }
    }

    tests.append(testDefinition);

    // update the timer if the list was empty
    if (tests.size() == 1)
    {
        updateTimer();
    }

    return tests.size()-1;
}

void Scheduler::Private::timeout()
{
    // get the test and execute it
    TestDefinitionPtr td = tests.at(0);
    q->execute(td);

    // remove it from the list
    tests.removeAt(0);
    testIds.remove(td->id());

    // check if it needs to be enqueued again or permanentely removed
    if (!td->timing()->reset())
    {
        emit q->testRemoved(td, 0);
        updateTimer();
    }
    else
    {
        // update the timer if another task is the next to run
        if (enqueue(td) != 0)
        {
            updateTimer();
        }
    }
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
    if (testDefinition->timing()->type() != "ondemand")
    {
        int pos = d->enqueue(testDefinition);
        emit testAdded(testDefinition, pos);
    }
    else
    {
        d->onDemandTests.append(testDefinition);
    }
}

void Scheduler::dequeue()
{
    // Whatever
}

void Scheduler::execute(const TestDefinitionPtr &testDefinition)
{
    d->executor->execute(testDefinition);
}

int Scheduler::executeOnDemandTest(const QUuid &id)
{
    foreach(TestDefinitionPtr td, d->onDemandTests)
    {
        if (td->id() == id)
        {
            execute(td);

            return 1;
        }
    }

    return -1;
}

#include "scheduler.moc"
