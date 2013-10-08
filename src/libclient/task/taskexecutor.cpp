#include "taskexecutor.h"

#include <QThread>

class InternalTaskExecutor : public QObject
{
    Q_OBJECT

public slots:
    void execute(const TestDefinition& test) {
        emit started(test);

        // TODO: Check the timing (too long ago?)
        // TODO: Run the test

        emit finished(test, ReportPtr());
    }

signals:
    void started(const TestDefinition& test);
    void finished(const TestDefinition& test, const ReportPtr& report);
};

class TaskExecutor::Private
{
public:
    Private(TaskExecutor* q)
    : q(q)
    , running(false)
    {
        // Should we really have a background thread running
        // all the time?
        taskThread.start();
        executor.moveToThread(&taskThread);

        connect(&executor, SIGNAL(started(TestDefinition)), q, SIGNAL(started(TestDefinition)));
        connect(&executor, SIGNAL(finished(TestDefinition)), q, SIGNAL(finished(TestDefinition)));
    }

    ~Private()
    {
        taskThread.quit();
        taskThread.wait();
    }

    TaskExecutor* q;

    bool running;
    QThread taskThread;
    InternalTaskExecutor executor;
};

TaskExecutor::TaskExecutor()
: d(new Private(this))
{

}

TaskExecutor::~TaskExecutor()
{

}

bool TaskExecutor::isRunning() const
{
    return d->running;
}

void TaskExecutor::execute(const TestDefinition &test)
{
    QMetaObject::invokeMethod(&d->executor, "execute", Qt::QueuedConnection, Q_ARG(TestDefinition, test));
}
