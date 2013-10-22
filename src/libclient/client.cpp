#include "client.h"
#include "controller/controlcontroller.h"
#include "network/networkmanager.h"
#include "task/taskexecutor.h"
#include "scheduler/schedulerstorage.h"
#include "report/reportstorage.h"
#include "scheduler/scheduler.h"
#include "settings.h"

#include <QNetworkAccessManager>

#include <QDebug>

// TEST INCLUDES
#include "timing/onofftiming.h"
#include "task/task.h"
#include "measurement/btc/btc_definition.h"
#include "measurement/ping/ping_definition.h"

class Client::Private : public QObject
{
    Q_OBJECT

public:
    Private(Client* q)
    : q(q)
    , status(Client::Unregistered)
    , networkAccessManager(new QNetworkAccessManager(q))
    , schedulerStorage(&scheduler)
    , reportStorage(&reportScheduler)
    {
        executor.setNetworkManager(&networkManager);
        scheduler.setExecutor(&executor);

        connect(&executor, SIGNAL(finished(TestDefinitionPtr,ResultPtr)), this, SLOT(taskFinished(TestDefinitionPtr,ResultPtr)));
    }

    Client* q;

    // Properties
    Client::Status status;
    QNetworkAccessManager* networkAccessManager;

    TaskExecutor executor;

    Scheduler scheduler;
    SchedulerStorage schedulerStorage;

    ReportScheduler reportScheduler;
    ReportStorage reportStorage;

    Settings settings;
    NetworkManager networkManager;

    ControlController controlController;

public slots:
    void taskFinished(const TestDefinitionPtr& test, const ResultPtr& result);
};

void Client::Private::taskFinished(const TestDefinitionPtr &test, const ResultPtr &result)
{
    ReportPtr oldReport = reportScheduler.reportByTaskId(test->id());

    ResultList results = oldReport.isNull() ? ResultList() : oldReport->results();
    results.append(result);

    ReportPtr report(new Report(test->id(), QDateTime::currentDateTime(), results));
    reportScheduler.addReport(report);
}

Client::Client(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

Client::~Client()
{
    d->schedulerStorage.storeData();
    d->reportStorage.storeData();
    delete d;
}

Client *Client::instance()
{
    static Client* ins = NULL;

    if ( !ins )
        ins = new Client();

    return ins;
}

bool Client::init()
{
    qRegisterMetaType<TestDefinitionPtr>();
    qRegisterMetaType<ResultPtr>();

    d->settings.init();

    // Initialize storages
    d->schedulerStorage.loadData();
    d->reportStorage.loadData();

    // Initialize controllers
    d->networkManager.init(&d->scheduler, &d->settings);
    d->controlController.init(&d->networkManager, &d->scheduler, &d->settings);

    return true;
}

void Client::btc()
{
    BulkTransportCapacityDefinition btcDef("141.82.59.143", 3365, 4096);

    TimingPtr timing(new OnOffTiming(QDateTime::currentDateTime().addSecs(5)));
    TestDefinitionPtr testDefinition(new TestDefinition(QUuid::createUuid(), "btc_ma", timing, btcDef.toVariant()));
    d->scheduler.enqueue(testDefinition);
}

void Client::upnp()
{
    TimingPtr timing(new OnOffTiming(QDateTime::currentDateTime().addSecs(5)));
    TestDefinitionPtr testDefinition(new TestDefinition("{3702e527-f84f-4542-8df6-4e3d2a0ec977}", "upnp", timing, QVariant()));
    d->scheduler.enqueue(testDefinition);
}

void Client::ping()
{
    PingDefinition pingDef("heise.de", 4, 1000);
    TimingPtr timing(new OnOffTiming(QDateTime::currentDateTime().addSecs(5)));
    TestDefinitionPtr testDefinition(new TestDefinition(QUuid::createUuid(), "ping", timing, pingDef.toVariant()));
    d->scheduler.enqueue(testDefinition);
}


void Client::setStatus(Client::Status status)
{
    if ( d->status == status )
        return;

    d->status = status;
    emit statusChanged();
}

Client::Status Client::status() const
{
    return d->status;
}

QNetworkAccessManager *Client::networkAccessManager() const
{
    return d->networkAccessManager;
}

Scheduler *Client::scheduler() const
{
    return &d->scheduler;
}

ReportScheduler *Client::reportScheduler() const
{
    return &d->reportScheduler;
}

NetworkManager *Client::networkManager() const
{
    return &d->networkManager;
}

TaskExecutor *Client::taskExecutor() const
{
    return &d->executor;
}

Settings *Client::settings() const
{
    return &d->settings;
}

#include "client.moc"
