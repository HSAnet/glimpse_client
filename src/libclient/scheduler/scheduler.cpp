#include "scheduler.h"
#include "../task/taskexecutor.h"

#include <QDir>
#include <QTimer>
#include <QCoreApplication>
#include <QDebug>
#include <QPointer>

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

    QPointer<TaskExecutor> executor;

    // Functions
    void updateTimer();
    int enqueue(const TestDefinitionPtr &testDefinition);

public slots:
    void timeout();
};

void Scheduler::Private::updateTimer()
{
    if (tests.isEmpty()) {
        timer.stop();
        qDebug() << "Scheduling timer stopped";
    } else {
        const TestDefinitionPtr& td = tests.at(0);
        int ms = td->timing()->timeLeft();
        if ( ms > 0 )
            timer.start( ms );
        else
            timeout();

        qDebug() << "Scheduling timer executes" << td->name() << "in" << ms << "ms";
    }
}

int Scheduler::Private::enqueue(const TestDefinitionPtr& testDefinition)
{
    bool wasEmpty = tests.isEmpty();

    int timeLeft = testDefinition->timing()->timeLeft();

    for(int i=0; i < tests.size(); ++i) {
        const TestDefinitionPtr& td = tests.at(i);
        if (timeLeft < td->timing()->timeLeft()) {
            tests.insert(i, testDefinition);

            if ( wasEmpty || i == 0 )
                updateTimer();

            return i;
        }
    }

    tests.append(testDefinition);

    if ( wasEmpty )
        updateTimer();

    return tests.size()-1;
}

void Scheduler::Private::timeout()
{
    for(int i=0; i < tests.size(); ++i) {
        TestDefinitionPtr td = tests.at(i);

        // We assume they are already sorted - WRONG, SO WRONG!
        if ( td->timing()->timeLeft() > 0 )
            continue;

        q->execute(td);

        tests.removeAt(i);

        if (!td->timing()->reset()) {
            emit q->testRemoved(td, i);

            if (tests.isEmpty())
                break;
            else
                --i;
        } else {
            int pos = enqueue(td);
            if (pos != i) {
                qDebug() << td->name() << "moved from" << i << "to" << pos;
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
