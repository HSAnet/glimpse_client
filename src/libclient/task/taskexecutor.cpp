#include "taskexecutor.h"
#include "../log/logger.h"
#include "../measurement/measurementfactory.h"
#include "../network/networkmanager.h"
#include "client.h"
#include "controller/ntpcontroller.h"
#include "localinformation.h"

#include <QThread>
#include <QPointer>
#include <QElapsedTimer>

LOGGER(TaskExecutor);

class InternalTaskExecutor : public QObject
{
    Q_OBJECT

public:
    MeasurementFactory factory;
    QPointer<NetworkManager> networkManager;

    ScheduleDefinition currentTest;
    MeasurementPtr measurement;
    QElapsedTimer timer;

private:
    LocalInformation localInformation;

public slots:
    void execute(const ScheduleDefinition &test, MeasurementObserver *observer)
    {
        LOG_INFO(QString("Preparing execution of %1").arg(test.name()));

        emit started(test);

        currentTest = test;
        measurement = factory.createMeasurement(test.name(), test.taskId());

        if (!measurement.isNull())
        {
            // get local information
            measurement->setPreInfo(localInformation.getVariables());

            connect(measurement.data(), SIGNAL(finished()), this, SLOT(measurementFinished()));
            connect(measurement.data(), SIGNAL(error(const QString &)), this, SLOT(measurementError(const QString &)));

            if (observer)
            {
                observer->created(measurement);
                delete observer;
            }

            if (networkManager->allInterfacesDown())
            {
                LOG_ERROR(QString("Unable to execute measurement, no interface is up: %1").arg(test.name()));
                measurementError("No interface is up");
                return;
            }

            MeasurementDefinitionPtr definition = factory.createMeasurementDefinition(test.name(), test.measurementDefinition());

            if (measurement->prepare(networkManager, definition))
            {
                // in case of no error this is the local information we want
                // because it is right before the actual measurement
                measurement->setPreInfo(localInformation.getVariables());
                measurement->setStartDateTime(Client::instance()->ntpController()->currentDateTime());
                timer.start();

                LOG_INFO(QString("Starting execution of %1").arg(test.name()));
                if (measurement->start())
                {
                    return;
                }
            }

            // the result should at least contain the errorString, as all the other
            // fields will be empty
            LOG_ERROR(QString("Finished execution of %1 (failed): %2").arg(test.name()).arg(measurement->errorString()));

            Result result;
            result.setStartDateTime(measurement->startDateTime());
            result.setEndDateTime(measurement->startDateTime().addMSecs(timer.elapsed()));
            result.setPreInfo(measurement->preInfo());
            result.setPostInfo(localInformation.getVariables());
            result.setErrorString(measurement->errorString());
            emit finished(test, result);

            measurement.clear();
        }
        else
        {
            LOG_ERROR(QString("Unable to create measurement: %1").arg(test.name()));
            emit finished(test, Result("Unable to create measurement"));
        }
    }

    void measurementFinished()
    {
        measurement->disconnect(this, SLOT(measurementFinished()));
        measurement->disconnect(this, SLOT(measurementError(const QString &)));

        LOG_INFO(QString("Finished execution of %1 (success)").arg(currentTest.name()));

        Result result = measurement->result();
        result.setStartDateTime(measurement->startDateTime());
        result.setEndDateTime(measurement->startDateTime().addMSecs(timer.elapsed()));
        result.setPreInfo(measurement->preInfo());
        result.setPostInfo(localInformation.getVariables());
        result.setErrorString(measurement->errorString()); // should be null

        emit finished(currentTest, result);
        measurement->stop();
        measurement.clear();
    }

    void measurementError(const QString &errorMsg)
    {
        measurement->disconnect(this, SLOT(measurementFinished()));
        measurement->disconnect(this, SLOT(measurementError(const QString &)));

        LOG_ERROR(QString("Finished execution of %1 (failed): %2").arg(currentTest.name()).arg(errorMsg));

        Result result;
        result.setStartDateTime(measurement->startDateTime());
        result.setEndDateTime(measurement->startDateTime().addMSecs(timer.elapsed()));
        result.setPreInfo(measurement->preInfo());
        result.setPostInfo(localInformation.getVariables());
        result.setErrorString(errorMsg);
        emit finished(currentTest, result);

        measurement->stop();
        measurement.clear();
    }

signals:
    void started(const ScheduleDefinition &test);
    void finished(const ScheduleDefinition &test, const Result &result);
};

class TaskExecutor::Private : public QObject
{
    Q_OBJECT

public:
    Private(TaskExecutor *q)
    : q(q)
    , running(false)
    {
        // Should we really have a background thread running
        // all the time?
        executor.moveToThread(&taskThread);
        taskThread.setObjectName("TaskExecutorThread");
        taskThread.start();

        connect(&executor, SIGNAL(started(ScheduleDefinition)), this, SLOT(onStarted()));
        connect(&executor, SIGNAL(finished(ScheduleDefinition, Result)), this, SLOT(onFinished()));

        connect(&executor, SIGNAL(started(ScheduleDefinition)), q, SIGNAL(started(ScheduleDefinition)));
        connect(&executor, SIGNAL(finished(ScheduleDefinition, Result)), q, SIGNAL(finished(ScheduleDefinition, Result)));
    }

    ~Private()
    {
        taskThread.quit();
        taskThread.wait();
    }

    struct QueueEntry
    {
        ScheduleDefinition definition;
        MeasurementObserver *observer;
    };

    TaskExecutor *q;

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
    if (!running)
    {
        running = true;
        emit q->runningChanged(running);
    }
}

Q_DECLARE_METATYPE(MeasurementObserver *);

void TaskExecutor::Private::onFinished()
{
    if (queue.isEmpty())
    {
        running = false;
        emit q->runningChanged(running);
    }
    else
    {
        QueueEntry entry = queue.takeFirst();
        QMetaObject::invokeMethod(&executor, "execute", Qt::QueuedConnection, Q_ARG(ScheduleDefinition, entry.definition),
                                  Q_ARG(MeasurementObserver *, entry.observer));
    }
}

TaskExecutor::TaskExecutor()
: d(new Private(this))
{
    qRegisterMetaType<MeasurementObserver *>();
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

void TaskExecutor::execute(const ScheduleDefinition &test, MeasurementObserver *observer)
{
    // Check preconditions
    if (!test.precondition().check())
    {
        LOG_INFO(QString("Measurement not executed, preconditions not met: %1").arg(test.name()));
        return;
    }

    // Abort if we are on a mobile connection
    if (d->executor.networkManager->onMobileConnection())
    {
        LOG_ERROR(QString("Unable to execute measurement, we are on a mobile connection or no interface is up: %1").arg(test.name()));
        return;
    }

    if (d->running == false)
    {
        d->running = true;
        emit runningChanged(d->running);

        QMetaObject::invokeMethod(&d->executor, "execute", Qt::QueuedConnection, Q_ARG(ScheduleDefinition, test),
                                  Q_ARG(MeasurementObserver *, observer));
    }
    else
    {
        TaskExecutor::Private::QueueEntry entry;
        entry.definition = test;
        entry.observer = observer;

        d->queue.append(entry);
    }
}

#include "taskexecutor.moc"
