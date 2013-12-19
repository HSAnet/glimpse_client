#include "taskexecutor.h"
#include "../log/logger.h"
#include "../measurement/measurementfactory.h"
#include "../network/networkmanager.h"

#include <QThread>
#include <QPointer>

LOGGER(TaskExecutor);

class InternalTaskExecutor : public QObject
{
    Q_OBJECT

public:
    MeasurementFactory factory;
    QPointer<NetworkManager> networkManager;

    TestDefinitionPtr currentTest;
    MeasurementPtr measurement;

public slots:
    void execute(const TestDefinitionPtr& test, MeasurementObserver* observer) {
        LOG_INFO(QString("Starting execution of %1").arg(test->name()));

        emit started(test);

        // TODO: Check the timing (too long ago?)
        currentTest = test;
        measurement = factory.createMeasurement(test->name());
        if ( !measurement.isNull() ) {
            connect(measurement.data(), SIGNAL(finished()), this, SLOT(measurementFinished()));

            if (observer) {
                observer->created(measurement);
                delete observer;
            }

            MeasurementDefinitionPtr definition = factory.createMeasurementDefinition(test->name(), test->measurementDefinition());

            if (measurement->prepare(networkManager, definition)) {
                if (measurement->start()) {
                    this->measurement = measurement;
                    return;
                }
            }
        } else {
            LOG_ERROR(QString("Unable to create measurement: %1").arg(test->name()));
        }

        LOG_INFO(QString("Finished execution of %1 (failed)").arg(test->name()));
        emit finished(test, ResultPtr());
    }

    void measurementFinished() {
        measurement->disconnect(this, SLOT(measurementFinished()));
        LOG_INFO(QString("Finished execution of %1 (success)").arg(currentTest->name()));
        emit finished(currentTest, measurement->result());
        measurement->stop();
        measurement.clear();
    }

signals:
    void started(const TestDefinitionPtr& test);
    void finished(const TestDefinitionPtr& test, const ResultPtr& result);
};

class TaskExecutor::Private : public QObject
{
    Q_OBJECT

public:
    Private(TaskExecutor* q)
    : q(q)
    , running(false)
    {
        // Should we really have a background thread running
        // all the time?
        executor.moveToThread(&taskThread);
        taskThread.setObjectName("TaskExecutorThread");
        taskThread.start();

        connect(&executor, SIGNAL(started(TestDefinitionPtr)), this, SLOT(onStarted()));
        connect(&executor, SIGNAL(finished(TestDefinitionPtr,ResultPtr)), this, SLOT(onFinished()));

        connect(&executor, SIGNAL(started(TestDefinitionPtr)), q, SIGNAL(started(TestDefinitionPtr)));
        connect(&executor, SIGNAL(finished(TestDefinitionPtr,ResultPtr)), q, SIGNAL(finished(TestDefinitionPtr,ResultPtr)));
    }

    ~Private()
    {
        taskThread.quit();
        taskThread.wait();
    }

    struct QueueEntry
    {
        TestDefinitionPtr definition;
        MeasurementObserver* observer;
    };

    TaskExecutor* q;

    // Properties
    bool running;
    QThread taskThread;
    InternalTaskExecutor executor;

    QList<QueueEntry> queue;

    // Functions

public slots:
    void onStarted();
    void onFinished();
};

void TaskExecutor::Private::onStarted()
{
    if (!running) {
        running = true;
        emit q->runningChanged(running);
    }
}

Q_DECLARE_METATYPE(MeasurementObserver*);

void TaskExecutor::Private::onFinished()
{
    if (queue.isEmpty()) {
        running = false;
        emit q->runningChanged(running);
    } else {
        QueueEntry entry = queue.takeFirst();
        QMetaObject::invokeMethod(&executor, "execute", Qt::QueuedConnection, Q_ARG(TestDefinitionPtr, entry.definition), Q_ARG(MeasurementObserver*, entry.observer));
    }
}

TaskExecutor::TaskExecutor()
: d(new Private(this))
{
    qRegisterMetaType<MeasurementObserver*>();
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

void TaskExecutor::execute(const TestDefinitionPtr &test, MeasurementObserver *observer)
{
    if (d->running == false) {
        d->running = true;
        emit runningChanged(d->running);

        QMetaObject::invokeMethod(&d->executor, "execute", Qt::QueuedConnection, Q_ARG(TestDefinitionPtr, test), Q_ARG(MeasurementObserver*, observer));
    } else {
        TaskExecutor::Private::QueueEntry entry;
        entry.definition = test;
        entry.observer = observer;

        d->queue.append(entry);
    }
}

#include "taskexecutor.moc"
