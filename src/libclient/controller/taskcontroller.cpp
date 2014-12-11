#include "taskcontroller.h"
#include "../network/networkmanager.h"
#include "../scheduler/scheduler.h"
#include "../settings.h"
#include "../webrequester.h"
#include "../network/requests/resourcerequest.h"
#include "../task/task.h"
#include "../task/taskvalidator.h"
#include "../log/logger.h"
#include "../timing/periodictiming.h"
#include "../client.h"
#include "../network/responses/getinstructionresponse.h"
#include "../network/responses/getscheduleresponse.h"
#include "../storage/storage.h"

#include <QCoreApplication>
#include <QPointer>
#include <QTimer>

LOGGER(TaskController);


class TaskController::Private : public QObject
{
    Q_OBJECT

public:
    Private(TaskController *q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), q, SLOT(fetchTasks()));
        connect(&instructionRequester, SIGNAL(error()), this, SLOT(instructionError()));
        connect(&instructionRequester, SIGNAL(finished()), this, SLOT(instructionFinished()));
        connect(&instructionRequester, SIGNAL(started()), q, SIGNAL(started()));
        connect(&instructionRequester, SIGNAL(error()), q, SIGNAL(error()));

        connect(&scheduleRequester, SIGNAL(error()), this, SLOT(scheduleError()));
        connect(&scheduleRequester, SIGNAL(finished()), this, SLOT(scheduleFinished()));

        instructionRequest.setPath(("/supervisor/api/v1/instruction/1/"));
        instructionRequester.setRequest(&instructionRequest);
        instructionRequester.setResponse(&instructionResponse);

        scheduleRequest.setPath("/supervisor/api/v1/schedule/");
        scheduleRequester.setRequest(&scheduleRequest);
        scheduleRequester.setResponse(&scheduleResponse);
    }

    TaskController *q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Scheduler> scheduler;
    QPointer<Settings> settings;

    WebRequester instructionRequester;
    GetResourceRequest instructionRequest;
    GetInstructionResponse instructionResponse;

    WebRequester scheduleRequester;
    GetResourceRequest scheduleRequest;
    GetScheduleResponse scheduleResponse;

    QTimer timer;

public slots:
    void updateTimer();
    void instructionFinished();
    void scheduleFinished();
    void instructionError();
    void scheduleError();
};

void TaskController::Private::updateTimer()
{
    // Set the new url
    QString newUrl = QString("https://%1").arg(Client::instance()->settings()->config()->supervisorAddress());

    if (instructionRequester.url() != newUrl)
    {
        LOG_DEBUG(QString("Instruction url set to %1").arg(newUrl));
        instructionRequester.setUrl(newUrl);
    }

    if (scheduleRequester.url() != newUrl)
    {
        LOG_DEBUG(QString("Schedule url set to %1").arg(newUrl));
        scheduleRequester.setUrl(newUrl);
    }

    TimingPtr timing = settings->config()->supervisorTiming();

    if (timing.isNull())
    {
        timer.stop();
        return;
    }

    QSharedPointer<PeriodicTiming> periodicTiming = timing.dynamicCast<PeriodicTiming>();
    Q_ASSERT(periodicTiming);

    int period = periodicTiming->interval();

    if (timer.interval() != period)
    {
        LOG_DEBUG(QString("Tasks schedule set to %1 sec.").arg(period / 1000));
        timer.setInterval(period);
    }

    timer.start();
}

void TaskController::Private::instructionFinished()
{
    // check if we received a kill signal
    if (instructionResponse.killSwitch() == "data")
    {
        Storage storage;
        storage.deleteData();

        LOG_INFO(QString("Removed all local data as instructed by the server."));
    }
    else if (instructionResponse.killSwitch() == "settings")
    {
        LOG_INFO(QString("Received kill signal from server, all data and settings will be deleted."));

        // remove data
        Storage storage;
        storage.deleteData();

        // remove settings
        settings->clear();

        // stop the probe
        LOG_INFO(QString("Quitting the application."));
        qApp->quit();
    }

    scheduleRequest.clearResourceIds();

    // check which schedules we need
    foreach (const ScheduleId &id, instructionResponse.scheduleIds())
    {
        if (!scheduler->knownTestId(id))
        {
            scheduleRequest.addResourceId(id.toInt());
        }
    }

    // check which schedules we need to remove
    foreach (const ScheduleId &id, instructionResponse.removeScheduleIds())
    {
        if (scheduler->knownTestId(id))
        {
            scheduler->dequeue(id);
        }
    }

    // get new schedules
    if(!scheduleRequest.resourceIds().isEmpty())
    {
        scheduleRequester.start();
    }
}

void TaskController::Private::scheduleFinished()
{
    foreach (const ScheduleDefinition &testDefinition, scheduleResponse.tasks())
    {
        scheduler->enqueue(testDefinition);
    }
}

void TaskController::Private::instructionError()
{
    LOG_ERROR(QString("Error fetching instructions: %1").arg(instructionRequester.errorString()));
}

void TaskController::Private::scheduleError()
{
    LOG_ERROR(QString("Error fetching schedules: %1").arg(scheduleRequester.errorString()));
}

TaskController::TaskController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

TaskController::~TaskController()
{
    delete d;
}

bool TaskController::init(NetworkManager *networkManager, Scheduler *scheduler, Settings *settings)
{
    d->scheduler = scheduler;
    d->settings = settings;
    d->networkManager = networkManager;

    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(updateTimer()));
    d->updateTimer();

    return true;
}

Controller::Status TaskController::status() const
{
    return (Status)d->instructionRequester.status();
}

QString TaskController::errorString() const
{
    return (QStringList() << d->instructionRequester.errorString() << d->scheduleRequester.errorString()).join("\n");
}

void TaskController::fetchTasks()
{
    d->instructionRequester.start();
}

#include "taskcontroller.moc"
