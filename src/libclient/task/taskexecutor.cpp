#include "taskexecutor.h"
#include "../measurement/measurementfactory.h"
#include "../network/networkmanager.h"

#include <QThread>
#include <QPointer>

class InternalTaskExecutor : public QObject
{
    Q_OBJECT

public:
    MeasurementFactory factory;
    QPointer<NetworkManager> networkManager;

public slots:
    void execute(const TestDefinitionPtr& test) {
        emit started(test);

        // TODO: Check the timing (too long ago?)

        MeasurementDefinitionPtr definition; // FIXME: read from test

        MeasurementPtr measurement = factory.createMeasurement(test->name());
        if ( !measurement.isNull() ) {
            measurement->prepare(networkManager, definition);

            measurement->start();
            measurement->stop();

            emit finished(test, measurement->result());
        } else {
            emit finished(test, ResultPtr());
        }
    }

signals:
    void started(const TestDefinitionPtr& test);
    void finished(const TestDefinitionPtr& test, const ResultPtr& result);
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

        connect(&executor, SIGNAL(started(TestDefinitionPtr)), q, SIGNAL(started(TestDefinitionPtr)));
        connect(&executor, SIGNAL(finished(TestDefinitionPtr,ResultPtr)), q, SIGNAL(finished(TestDefinitionPtr,ResultPtr)));
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
    delete d;
}

void TaskExecutor::setNetworkManager(NetworkManager *networkManager)
{
    d->executor.networkManager = networkManager;
}

NetworkManager *TaskExecutor::networkManager() const
{
    return d->executor.networkManager;
}

bool TaskExecutor::isRunning() const
{
    return d->running;
}

void TaskExecutor::execute(const TestDefinitionPtr &test)
{
    QMetaObject::invokeMethod(&d->executor, "execute", Qt::QueuedConnection, Q_ARG(TestDefinitionPtr, test));
}

#include "taskexecutor.moc"
