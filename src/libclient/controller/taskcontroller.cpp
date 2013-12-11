#include "taskcontroller.h"
#include "../network/networkmanager.h"
#include "../scheduler/scheduler.h"
#include "../settings.h"
#include "../webrequester.h"
#include "../network/requests/gettasksrequest.h"
#include "../task/task.h"
#include "../log/logger.h"

#include <QPointer>

LOGGER(TaskController);

class GetTasksResponse : public Response
{
    Q_OBJECT

public:
    GetTasksResponse(QObject* parent = 0)
    : Response(parent)
    {
    }

    TestDefinitionList tasks() const {
        return m_tasks;
    }

    bool fillFromVariant(const QVariantMap &variant) {
        m_tasks.clear();

        QVariantList tasks = variant.value("tasks").toList();
        foreach(const QVariant& taskVariant, tasks) {
            TestDefinitionPtr test = TestDefinition::fromVariant(taskVariant);
            if (test)
                m_tasks.append(test);
        }

        LOG_DEBUG(QString("Received %1 tasks").arg(m_tasks.size()));

        return true;
    }

protected:
    TestDefinitionList m_tasks;
};

class TaskController::Private : public QObject
{
    Q_OBJECT

public:
    Private(TaskController* q)
    : q(q)
    {
        connect(&requester, SIGNAL(error()), this, SLOT(error()));
        connect(&requester, SIGNAL(finished()), this, SLOT(finished()));
        connect(&requester, SIGNAL(statusChanged(Status)), q, SIGNAL(statusChanged()));
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

public slots:
    void finished();
    void error();
};

void TaskController::Private::finished()
{
    foreach(const TestDefinitionPtr& testDefinition, response.tasks()) {
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
