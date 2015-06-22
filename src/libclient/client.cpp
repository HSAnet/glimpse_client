#include "client.h"
#include "controller/taskcontroller.h"
#include "controller/reportcontroller.h"
#include "controller/logincontroller.h"
#include "controller/configcontroller.h"
#include "controller/crashcontroller.h"
#include "controller/ntpcontroller.h"
#include "network/networkmanager.h"
#include "task/taskexecutor.h"
#include "scheduler/schedulerstorage.h"
#include "report/reportstorage.h"
#include "scheduler/scheduler.h"
#include "settings.h"
#include "log/logger.h"
#include "types.h"
#include "trafficbudgetmanager.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QDebug>
#include <QHostInfo>

#ifdef Q_OS_UNIX
#include <QSocketNotifier>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <controller/mplanecontroller.h>
#endif // Q_OS_UNIX

#ifdef Q_OS_WIN
#include <Windows.h>
#include <stdio.h>
#endif // Q_OS_WIN

#include "timing/immediatetiming.h"
#include "timing/ondemandtiming.h"
#include "timing/timing.h"
#include "task/task.h"
#include "measurement/btc/btc_definition.h"
#include "measurement/http/httpdownload_definition.h"
#include "measurement/dnslookup/dnslookup_definition.h"
#include "measurement/reverse_dnslookup/reverseDnslookup_definition.h"
#include "measurement/packettrains/packettrainsdefinition.h"
#include "measurement/ping/ping_definition.h"
#include "measurement/traceroute/traceroute_definition.h"

LOGGER(Client);

class Client::Private : public QObject
{
    Q_OBJECT

public:
    Private(Client *q)
    : q(q)
    , status(Client::Unregistered)
    , networkAccessManager(new QNetworkAccessManager(q))
    , schedulerStorage(&scheduler)
    , reportStorage(&reportScheduler)
    {
        executor.setNetworkManager(&networkManager);
        scheduler.setExecutor(&executor);

        connect(&executor, SIGNAL(finished(ScheduleDefinition, Result)), this, SLOT(taskFinished(ScheduleDefinition, Result)));
        connect(&loginController, SIGNAL(finished()), this, SLOT(loginStatusChanged()));
    }

    Client *q;

    // Properties
    Client::Status status;
    QNetworkAccessManager *networkAccessManager;

    TaskExecutor executor;

    Scheduler scheduler;
    SchedulerStorage schedulerStorage;

    ReportScheduler reportScheduler;
    ReportStorage reportStorage;

    Settings settings;
    NetworkManager networkManager;

    TaskController taskController;
    ReportController reportController;
    LoginController loginController;
    ConfigController configController;
    CrashController crashController;
    NtpController ntpController;
    MPlaneController mPlaneController;

    TrafficBudgetManager trafficBudgetManager;

#ifdef Q_OS_UNIX
    static int sigintFd[2];
    static int sighupFd[2];
    static int sigtermFd[2];

    QSocketNotifier *snInt;
    QSocketNotifier *snHup;
    QSocketNotifier *snTerm;

    // Unix signal handlers.
    static void intSignalHandler(int unused);
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);
#endif // Q_OS_UNIX

    // Functions
    void setupUnixSignalHandlers();

#ifdef Q_OS_WIN
    static BOOL CtrlHandler(DWORD ctrlType);
#endif // Q_OS_WIN

public slots:
#ifdef Q_OS_UNIX
    void handleSigInt();
    void handleSigHup();
    void handleSigTerm();
#endif // Q_OS_UNIX
    void taskFinished(const ScheduleDefinition &test, const Result &result);
    void loginStatusChanged();
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
    {
        return;
    }

    hup.sa_handler = Client::Private::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, 0) > 0)
    {
        return;
    }

    term.sa_handler = Client::Private::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = 0;
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
    {
        return;
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
    {
        qFatal("Couldn't create INT socketpair");
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
    {
        qFatal("Couldn't create HUP socketpair");
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
    {
        qFatal("Couldn't create TERM socketpair");
    }

    snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));

    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));

    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
#elif defined(Q_OS_WIN)
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)Private::CtrlHandler, TRUE);
#endif
}

#ifdef Q_OS_WIN
BOOL Client::Private::CtrlHandler(DWORD ctrlType)
{
    switch (ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
        LOG_INFO("Close requested, quitting.");
        qApp->quit();
        return TRUE;

    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        LOG_INFO("System shutdown or user logout, quitting.");
        qApp->quit();
        return FALSE;

    default:
        return FALSE;
    }
}
#endif // Q_OS_WIN

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

void Client::Private::taskFinished(const ScheduleDefinition &test, const Result &result)
{
    Report report = reportScheduler.reportByTaskId(test.taskId());

    ResultList results = report.isNull() ? ResultList() : report.results();
    results.append(result);

    if (report.isNull() || results.size() == 1)
    {
        report = Report(test.taskId(), Client::instance()->ntpController()->currentDateTime(), Client::version(), results, test.specification());
        reportScheduler.addReport(report);
    }
    else
    {
        report.setResults(results);
        reportScheduler.modifyReport(report);
    }
}

void Client::Private::loginStatusChanged()
{
    if (!settings.isPassive() && loginController.registeredDevice())
    {
        taskController.fetchTasks();
    }
}

Client::Client(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

Client::~Client()
{
    d->schedulerStorage.storeData();
    // don't write to local storage to avoid duplicates
    d->reportStorage.storeData(false);
    delete d;
}

Client *Client::instance()
{
    static Client *ins = NULL;

    if (!ins)
    {
        ins = new Client();
    }

    return ins;
}

bool Client::init()
{
    qRegisterMetaType<ScheduleDefinition>();
    qRegisterMetaType<Result>();

    d->setupUnixSignalHandlers();
    d->settings.init();

    // Initialize storages
    //d->schedulerStorage.loadData();
    //d->reportStorage.loadData();

    // Initialize controllers
    d->networkManager.init(&d->scheduler, &d->settings);
    //d->configController.init(&d->networkManager, &d->settings);
    d->reportController.init(&d->reportScheduler, &d->settings);
    //d->loginController.init(&d->networkManager, &d->settings);
    //d->crashController.init(&d->networkManager, &d->settings);
    d->ntpController.init();
    d->trafficBudgetManager.init();
    d->mPlaneController.init(&d->networkManager, &d->scheduler, &d->settings);

    /*
    if (!d->settings.isPassive())
    {
        d->taskController.init(&d->networkManager, &d->scheduler, &d->settings);
    }

    if (d->crashController.hasCrashReports())
    {
        d->crashController.uploadReports();
    }

    // add on demand tasks
    ScheduleDefinitionList tests;
    TimingPtr timing(new OnDemandTiming());
    Precondition precondition;

    tests.append(ScheduleDefinition(ScheduleId(1), TaskId(1), "ping", timing, PingDefinition("measure-it.net", 4, 200, 4000, 64, 33434, 33434, 74,
                                                                                             ping::System).toVariant(), precondition));
    tests.append(ScheduleDefinition(ScheduleId(2), TaskId(2), "btc_ma", timing, BulkTransportCapacityDefinition("141.82.57.241", 5106, 1048576,
                                                                                     10).toVariant(), precondition));
    tests.append(ScheduleDefinition(ScheduleId(3), TaskId(3), "btc_ma", timing, BulkTransportCapacityDefinition("141.82.57.241", 5106, 1048576,
                                                                                     10).toVariant(), precondition)); // this is for btc upload which is not implemented yet
    tests.append(ScheduleDefinition(ScheduleId(4), TaskId(4), "ping", timing, PingDefinition("measure-it.net", 4, 200, 1000, 64, 33434, 33434, 74,
                                                                  ping::Udp).toVariant(), precondition));
    tests.append(ScheduleDefinition(ScheduleId(5), TaskId(5), "ping", timing, PingDefinition("measure-it.net", 4, 200, 1000, 64, 33434, 33434, 74,
                                                                  ping::Tcp).toVariant(), precondition));
    tests.append(ScheduleDefinition(ScheduleId(6), TaskId(6), "dnslookup", timing, DnslookupDefinition("measure-it.net").toVariant(), precondition));
    tests.append(ScheduleDefinition(ScheduleId(7), TaskId(7), "httpdownload", timing,
                                HTTPDownloadDefinition("http://www.measure-it.net:80/static/measurement/67108864", false, 1, 10000, 3000, 1000).toVariant(),
                                precondition));
    tests.append(ScheduleDefinition(ScheduleId(8), TaskId(8), "packettrains_ma", timing, PacketTrainsDefinition("141.82.57.241", 5106, 1000, 48, 1,
                                                                                     10485760, 262144000, 200000000).toVariant(),
                                precondition));
    tests.append(ScheduleDefinition(ScheduleId(9), TaskId(9), "reversednslookup", timing, ReverseDnslookupDefinition("141.82.57.241").toVariant(),
                                precondition));
    tests.append(ScheduleDefinition(ScheduleId(10), TaskId(10), "traceroute", timing, TracerouteDefinition("measure-it.net", 3, 1000, 1000, 33434,
                                                                               33434, 74, ping::Udp).toVariant(),
                                precondition));
    tests.append(ScheduleDefinition(ScheduleId(11), TaskId(11), "upnp", timing, QVariant(), precondition));
    tests.append(ScheduleDefinition(ScheduleId(12), TaskId(12), "wifilookup", timing, QVariant(), precondition));

    foreach (const ScheduleDefinition &test, tests)
    {
        d->scheduler.enqueue(test);
    }
    */

    return true;
}

bool Client::autoLogin()
{
    if (d->settings.hasLoginData())
    {
        // check if api key is still valid
        d->loginController.checkApiKey();

        return true;
    }

    return false;
}

void Client::btc()
{
    d->scheduler.executeOnDemandTest(ScheduleId(2));
}

void Client::btc(const QString &host)
{
    BulkTransportCapacityDefinition btcDef(host, 5106, 1024 * 1024, 10);
    TimingPtr timing(new ImmediateTiming());
    ScheduleDefinition testDefinition(ScheduleId(2), TaskId(2), "btc_ma", timing,
                                  btcDef.toVariant(), Precondition());
    d->scheduler.enqueue(testDefinition);
}

void Client::http()
{
    d->scheduler.executeOnDemandTest(ScheduleId(7));
}

void Client::http(const QString &url, bool avoidCaches, int threads, int targetTime,
                  int rampUpTime, int slotLength)
{
    HTTPDownloadDefinition httpDef(url, avoidCaches, threads , targetTime, rampUpTime, slotLength);
    TimingPtr timing(new ImmediateTiming());
    ScheduleDefinition testDefinition(ScheduleId(7), TaskId(7), "httpdownload", timing,
                                  httpDef.toVariant(), Precondition());
    d->scheduler.enqueue(testDefinition);
}

void Client::upnp()
{
    d->scheduler.executeOnDemandTest(ScheduleId(11));
}

void Client::dnslookup()
{
    d->scheduler.executeOnDemandTest(ScheduleId(6));
}

void Client::reverseDnslookup()
{
    d->scheduler.executeOnDemandTest(ScheduleId(9));
}

void Client::packetTrains()
{
    d->scheduler.executeOnDemandTest(ScheduleId(8));
}

void Client::packetTrains(QString host, quint16 port, quint16 packetSize, quint16 trainLength, quint8 iterations,
                          quint64 rateMin, quint64 rateMax, quint64 delay)
{
    PacketTrainsDefinition packetTrainsDef(host, port, packetSize, trainLength, iterations, rateMin, rateMax, delay);

    TimingPtr timing(new ImmediateTiming());
    ScheduleDefinition testDefinition(ScheduleId(8), TaskId(8), "packettrains_ma", timing,
                                  packetTrainsDef.toVariant(), Precondition());
    d->scheduler.enqueue(testDefinition);
}

void Client::ping()
{
    d->scheduler.executeOnDemandTest(ScheduleId(1));
}

void Client::ping(const QString &url, const quint32 &count, const quint32 &interval, const quint32 &receiveTimeout,
                  const int &ttl, const quint16 &destinationPort, const quint16 &sourcePort, const quint32 &payload,
                  const QString &type)
{
    PingDefinition pingDef(url, count, interval, receiveTimeout, ttl, destinationPort, sourcePort, payload,
                           pingTypeFromString(type));

    TimingPtr timing(new ImmediateTiming());
    ScheduleDefinition testDefinition(ScheduleId(1), TaskId(1), "ping", timing,
                                  pingDef.toVariant(), Precondition());
    d->scheduler.enqueue(testDefinition);
}

void Client::traceroute()
{
    d->scheduler.executeOnDemandTest(ScheduleId(10));
}

void Client::traceroute(const QString &url,
                        const quint32 &count,
                        const quint32 &interval,
                        const quint32 &receiveTimeout,
                        const quint16 &destinationPort,
                        const quint16 &sourcePort,
                        const quint32 &payload,
                        const QString &type)
{
    TracerouteDefinition tracerouteDef(url, count, interval, receiveTimeout, destinationPort, sourcePort, payload,
                                       pingTypeFromString(type));

    TimingPtr timing(new ImmediateTiming());
    ScheduleDefinition testDefinition(ScheduleId(10), TaskId(10), "traceroute", timing,
                                  tracerouteDef.toVariant(), Precondition());
    d->scheduler.enqueue(testDefinition);
}

void Client::wifiLookup()
{
    d->scheduler.executeOnDemandTest(ScheduleId(12));
}

void Client::measureIt()
{
    // System Ping
    d->scheduler.executeOnDemandTest(ScheduleId(1));

    // BTC
    d->scheduler.executeOnDemandTest(ScheduleId(2));

    // HTTP download
    d->scheduler.executeOnDemandTest(ScheduleId(7));

    // PacketTrains
    d->scheduler.executeOnDemandTest(ScheduleId(8));

    // Traceroute
    d->scheduler.executeOnDemandTest(ScheduleId(10));
}

void Client::setStatus(Client::Status status)
{
    if (d->status == status)
    {
        return;
    }

    d->status = status;
    emit statusChanged();
}

Client::Status Client::status() const
{
    return d->status;
}

QString Client::version()
{
    return QString("%1.%2.%3").arg(Client::versionMajor).arg(Client::versionMinor).arg(Client::versionPatch);
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

ConfigController *Client::configController() const
{
    return &d->configController;
}

LoginController *Client::loginController() const
{
    return &d->loginController;
}

ReportController *Client::reportController() const
{
    return &d->reportController;
}

TaskController *Client::taskController() const
{
    return &d->taskController;
}

CrashController *Client::crashController() const
{
    return &d->crashController;
}

NtpController *Client::ntpController() const
{
    return &d->ntpController;
}

MPlaneController *Client::mPlaneController() const
{
    return &d->mPlaneController;
}

Settings *Client::settings() const
{
    return &d->settings;
}

TrafficBudgetManager *Client::trafficBudgetManager() const
{
    return &d->trafficBudgetManager;
}

#include "client.moc"
