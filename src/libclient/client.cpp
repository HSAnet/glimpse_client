#include "client.h"
#include "controller/controlcontroller.h"
#include "network/networkmanager.h"
#include "task/taskexecutor.h"
#include "scheduler/schedulerstorage.h"
#include "report/reportstorage.h"
#include "scheduler/scheduler.h"
#include "settings.h"
#include "log/logger.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QDebug>

#ifdef Q_OS_UNIX
#include <QSocketNotifier>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#endif // Q_OS_UNIX

// TEST INCLUDES
#include "timing/onofftiming.h"
#include "task/task.h"
#include "measurement/btc/btc_definition.h"
#include "measurement/ping/ping_definition.h"

LOGGER(Client);

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

#ifdef Q_OS_UNIX
    static int sigintFd[2];
    static int sighupFd[2];
    static int sigtermFd[2];

    QSocketNotifier* snInt;
    QSocketNotifier* snHup;
    QSocketNotifier* snTerm;

    // Unix signal handlers.
    static void intSignalHandler(int unused);
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);
#endif // Q_OS_UNIX

    // Functions
    void setupUnixSignalHandlers();

public slots:
#ifdef Q_OS_UNIX
    void handleSigInt();
    void handleSigHup();
    void handleSigTerm();
#endif // Q_OS_UNIX
    void taskFinished(const TestDefinitionPtr& test, const ResultPtr& result);
};

#ifdef Q_OS_UNIX
int Client::Private::sigintFd[2];
int Client::Private::sighupFd[2];
int Client::Private::sigtermFd[2];

void Client::Private::intSignalHandler(int)
{
    char a = 1;
    ::write(sigintFd[0], &a, sizeof(a));
}

void Client::Private::hupSignalHandler(int)
{
    char a = 1;
    ::write(sighupFd[0], &a, sizeof(a));
}

void Client::Private::termSignalHandler(int)
{
    char a = 1;
    ::write(sigtermFd[0], &a, sizeof(a));
}
#endif // Q_OS_UNIX

void Client::Private::setupUnixSignalHandlers()
{
#ifdef Q_OS_UNIX
    struct sigaction Int, hup, term;

    Int.sa_handler = Client::Private::intSignalHandler;
    sigemptyset(&Int.sa_mask);
    Int.sa_flags = 0;
    Int.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &Int, 0) > 0)
       return;

    hup.sa_handler = Client::Private::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, 0) > 0)
       return;

    term.sa_handler = Client::Private::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
       return;

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
       qFatal("Couldn't create INT socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
       qFatal("Couldn't create HUP socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
       qFatal("Couldn't create TERM socketpair");

    snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));

    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));

    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
#endif // Q_OS_UNIX
}

#ifdef Q_OS_UNIX
void Client::Private::handleSigInt()
{
    snInt->setEnabled(false);
    char tmp;
    ::read(sigintFd[1], &tmp, sizeof(tmp));

    LOG_INFO("Interrupt requested, quitting.");
    qApp->quit();

    snInt->setEnabled(true);
}

void Client::Private::handleSigTerm()
{
    snTerm->setEnabled(false);
    char tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));

    LOG_INFO("Termination requested, quitting.");
    qApp->quit();

    snTerm->setEnabled(true);
}

void Client::Private::handleSigHup()
{
    snHup->setEnabled(false);
    char tmp;
    ::read(sighupFd[1], &tmp, sizeof(tmp));

    LOG_INFO("Hangup detected, quitting.");
    qApp->quit();

    snHup->setEnabled(true);
}
#endif // Q_OS_UNIX

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

    d->setupUnixSignalHandlers();
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
    BulkTransportCapacityDefinition btcDef("141.82.49.80", 3365, 1024*50);

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
    PingDefinition pingDef("141.82.49.80", 4, 2);
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
