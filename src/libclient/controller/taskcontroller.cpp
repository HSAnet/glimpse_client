#include "taskcontroller.h"
#include "../network/networkmanager.h"
#include "../scheduler/scheduler.h"
#include "../settings.h"
#include "../webrequester.h"
#include "../network/requests/gettasksrequest.h"
#include "../task/task.h"
#include "../task/taskvalidator.h"
#include "../log/logger.h"
#include "../timing/periodictiming.h"
#include "../client.h"

#include <QPointer>
#include <QTimer>

LOGGER(TaskController);

class GetTasksResponse : public Response
{
    Q_OBJECT

public:
    GetTasksResponse(QObject* parent = 0)
    : Response(parent)
    {
    }

    TestDefinitionList tasks() const
    {
        return m_tasks;
    }

    bool fillFromVariant(const QVariantMap &variant)
    {
        m_tasks.clear();

        QVariantList tasks = variant.value("tasks").toList();
        foreach(const QVariant& taskVariant, tasks)
        {
            TestDefinitionPtr test = TestDefinition::fromVariant(taskVariant);

            if (m_validator.validate(test) == TaskValidator::Valid)
            {
                m_tasks.append(test);
            }
            else
            {
                // TODO: Get more information
                LOG_DEBUG(QString("Received invalid task, ignoring."));
            }
        }

        LOG_DEBUG(QString("Received %1 tasks").arg(m_tasks.size()));

        return true;
    }

protected:
    TaskValidator m_validator;
    TestDefinitionList m_tasks;
};

class TaskController::Private : public QObject
{
    Q_OBJECT

public:
    Private(TaskController* q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), q, SLOT(fetchTasks()));
        connect(&requester, SIGNAL(error()), this, SLOT(error()));
        connect(&requester, SIGNAL(finished()), this, SLOT(finished()));
        connect(&requester, SIGNAL(statusChanged(WebRequester::Status)), q, SIGNAL(statusChanged()));
        connect(&requester, SIGNAL(started()), q, SIGNAL(started()));
        connect(&requester, SIGNAL(finished()), q, SIGNAL(finished()));
        connect(&requester, SIGNAL(error()), q, SIGNAL(error()));

        requester.setRequest(&request);
        requester.setResponse(&response);
    }

    TaskController* q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Scheduler> scheduler;
    QPointer<Settings> settings;

    WebRequester requester;
    GetTasksRequest request;
    GetTasksResponse response;

    QTimer timer;

public slots:
    void updateTimer();
    void finished();
    void error();
};

void TaskController::Private::updateTimer()
{
    // Set the new url
    QString newUrl = QString("http://%1").arg(Client::instance()->settings()->config()->supervisorAddress());

    if (requester.url() != newUrl)
    {
        LOG_INFO(QString("Task url set to %1").arg(newUrl));
        requester.setUrl(newUrl);
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
        LOG_INFO(QString("Tasks schedule set to %1 sec.").arg(period/1000));
        timer.setInterval(period);
    }

    timer.start();
}

void TaskController::Private::finished()
{
    foreach(const TestDefinitionPtr& testDefinition, response.tasks())
    {
        scheduler->enqueue(testDefinition);
    }
}

void TaskController::Private::error()
{
    LOG_ERROR(QString("Error fetching tasks: %1").arg(requester.errorString()));
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
    return (Status)d->requester.status();
}

QString TaskController::errorString() const
{
    return d->requester.errorString();
}

void TaskController::fetchTasks()
{
    d->requester.start();
}

#include "taskcontroller.moc"
