#include "scheduler.h"
#include "../task/taskexecutor.h"
#include "../log/logger.h"
#include "../timing/ondemandtiming.h"
#include "client.h"
#include "controller/ntpcontroller.h"

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
    Private(Scheduler *q)
    : q(q)
    , path(qApp->applicationDirPath())
    {
        timer.setSingleShot(true);
        timer.setTimerType(Qt::PreciseTimer);
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

    Scheduler *q;

    // Properties
    QDir path;
    QTimer timer;

    ScheduleDefinitionList tests;
    ScheduleDefinitionList onDemandTests;
    TaskList tasks;
    QSet<ScheduleId> testIds;
    QSet<ScheduleId> onDemandTestIds;
    QSet<ScheduleId> allTestIds;
    QSet<TaskId> taskIds;

    QPointer<TaskExecutor> executor;

    // Functions
    void updateTimer();
    int enqueue(const ScheduleDefinition &testDefinition);
    int dequeue(const ScheduleId &id);

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
        const ScheduleDefinition td = tests.at(0);
        qint64 ms = td.timing()->timeLeft();

        if (ms > 0)
        {
            LOG_DEBUG(QString("Scheduling timer executes %1 in %2 ms").arg(td.name()).arg(ms));
            timer.start(ms);
        }
        else
        {
            // If we would call timeout() directly, the testAdded() signal
            // would be emitted after execution.
            LOG_DEBUG(QString("Scheduling timer executes %1 now").arg(td.name()));
            timer.start(100); // wait 100 ms before executing
        }
    }
}

int Scheduler::Private::enqueue(const ScheduleDefinition &testDefinition)
{
    // abort if test-id is already in scheduler or if the test has no next run time
    if (testIds.contains(testDefinition.id()) || !testDefinition.timing()->nextRun().isValid())
    {
        return -1;
    }

    qint64 timeLeft = testDefinition.timing()->timeLeft();

    for (int i = 0; i < tests.size(); i++)
    {
        const ScheduleDefinition &td = tests.at(i);

        if (timeLeft < td.timing()->timeLeft())
        {
            tests.insert(i, testDefinition);
            testIds.insert(testDefinition.id());
            allTestIds.insert(testDefinition.id());

            // update the timer if this is the new first element
            if (i == 0)
            {
                updateTimer();
            }

            return i;
        }
    }

    tests.append(testDefinition);
    testIds.insert(testDefinition.id());

    // update the timer if the list was empty
    if (tests.size() == 1)
    {
        updateTimer();
    }

    return tests.size() - 1;
}

int Scheduler::Private::dequeue(const ScheduleId &id)
{
    ScheduleDefinition td;
    td.setId(id);

    int position = tests.indexOf(td);
    tests.removeAt(position);
    testIds.remove(id);
    allTestIds.remove(id);

    emit q->testRemoved(td, position);

    if (position == 0) // if this was the next Test to schedule update the timer
    {
        updateTimer();
    }

    return position;
}

void Scheduler::Private::timeout()
{
    // get the test and execute it
    ScheduleDefinition td = tests.at(0);

    QDateTime t = td.timing()->lastExecution();

    // don't schedule if this measurement was executed in the last 1,5s
    if (t.isValid() && t.msecsTo(Client::instance()->ntpController()->currentDateTime()) < 1500)
    {
        LOG_DEBUG("Scheduler timeout to soon after last execution, skipping.")
        updateTimer();
        return;
    }

    // remove it from the list
    tests.removeAt(0);
    testIds.remove(td.id());
    allTestIds.remove(td.id());

    q->execute(td);

    // check if it needs to be enqueued again or permanentely removed
    if (!td.timing()->reset())
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

ScheduleDefinitionList Scheduler::tests() const
{
    return d->tests;
}

TaskList Scheduler::tasks() const
{
    return d->tasks;
}

void Scheduler::addTask(const Task &task)
{
    if (!d->taskIds.contains(task.id()))
    {
        d->tasks.append(task);
        emit taskAdded(task);
        d->taskIds.insert(task.id());
    }
}

void Scheduler::enqueue(const ScheduleDefinition &testDefinition)
{
    if (testDefinition.timing()->type() != "ondemand")
    {
        int pos = d->enqueue(testDefinition);

        emit testAdded(testDefinition, pos);
    }
    else
    {
        d->onDemandTests.append(testDefinition);
        d->onDemandTestIds.insert(testDefinition.id());
        d->allTestIds.insert(testDefinition.id());
    }

    addTask(Task::fromVariant(testDefinition.task()));
}

void Scheduler::dequeue(const ScheduleId &id)
{
    d->dequeue(id);
}

void Scheduler::execute(const ScheduleDefinition &testDefinition)
{
    d->executor->execute(testDefinition);
}

int Scheduler::executeOnDemandTest(const ScheduleId &id)
{
    foreach (const ScheduleDefinition& td, d->onDemandTests)
    {
        if (td.id() == id)
        {
            execute(td);

            return 1;
        }
    }

    LOG_ERROR("Test definition not found.");

    return -1;
}

bool Scheduler::knownTestId(const ScheduleId &id)
{
    foreach (const ScheduleId &testId, d->allTestIds)
    {
        if (testId == id)
        {
            return true;
        }
    }

    return false;
}

#include "scheduler.moc"
