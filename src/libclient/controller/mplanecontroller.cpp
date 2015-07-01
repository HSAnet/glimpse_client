#include "mplanecontroller.h"
#include "webrequester.h"
#include "network/networkmanager.h"
#include "network/requests/postrequest.h"
#include "network/requests/getrequest.h"
#include "network/responses/specificationresponse.h"
#include "network/responses/capabilityresponse.h"
#include "timing/timer.h"
#include "settings.h"
#include "log/logger.h"
#include "scheduler/scheduler.h"
#include "timing/periodictiming.h"
#include "measurement/ping/ping_definition.h"
#include "measurement/http/httpdownload_definition.h"
#include "measurement/dnslookup/dnslookup_definition.h"
#include "measurement/traceroute/traceroute_definition.h"
#include "measurement/reverse_dnslookup/reverseDnslookup_definition.h"

#include <QPointer>
#include <QCoreApplication>

LOGGER(MplaneController);

static TimingPtr TIMING(new PeriodicTiming(30000));

class MPlaneController::Private : public QObject
{
    Q_OBJECT

public:
    Private(MPlaneController *q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), q, SLOT(fetchSpecifications()));
        connect(&timer, SIGNAL(timeout()), q, SLOT(sendCapabilities()));
        connect(&capabilityRequester, SIGNAL(error()), this, SLOT(capabilitiesError()));
        connect(&capabilityRequester, SIGNAL(finished()), this, SLOT(capabilitiesFinished()));
        connect(&capabilityRequester, SIGNAL(started()), q, SIGNAL(started()));
        connect(&capabilityRequester, SIGNAL(error()), q, SIGNAL(error()));
        connect(&specificationRequester, SIGNAL(error()), this, SLOT(specificationsError()));
        connect(&specificationRequester, SIGNAL(finished()), this, SLOT(specificationsFinished()));
        connect(&specificationRequester, SIGNAL(started()), q, SIGNAL(started()));
        connect(&specificationRequester, SIGNAL(error()), q, SIGNAL(error()));
    }

    MPlaneController *q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Scheduler> scheduler;
    QPointer<Settings> settings;

    WebRequester capabilityRequester;
    WebRequester specificationRequester;
    PostRequest postRequest;
    GetRequest getRequest;
    SpecificationResponse specificationResponse;
    CapabilityResponse capabilityResponse;


    Timer timer;

public slots:
    void updateTimer();
    void capabilitiesError();
    void capabilitiesFinished();
    void specificationsError();
    void specificationsFinished();
};

void MPlaneController::Private::updateTimer()
{
    QString newUrl = QString("http://%1").arg(settings->config()->supervisorAddress());

    if (capabilityRequester.url() != newUrl)
    {
        LOG_DEBUG(QString("Supervisor url set to %1").arg(newUrl));
        capabilityRequester.setUrl(newUrl);
        specificationRequester.setUrl(newUrl);
    }

    TimingPtr timing = TIMING;

    if (timing.isNull())
    {
        timer.stop();
        return;
    }

    // send capabilities after updating the timer
    q->sendCapabilities();

    timer.setTiming(timing);
    timer.start();
}

void MPlaneController::Private::capabilitiesError()
{
    LOG_ERROR(QString("Error sending capabilities: %1").arg(capabilityRequester.errorString()));
}

void MPlaneController::Private::capabilitiesFinished()
{
    LOG_INFO("Capabilities sent"); // TODO response from server?
    capabilityRequester.disconnect();

    q->fetchSpecifications();
}

void MPlaneController::Private::specificationsError()
{
    LOG_ERROR(QString("Error retrieving specifications: %1").arg(capabilityRequester.errorString()));
}

void MPlaneController::Private::specificationsFinished()
{
    LOG_INFO(QString("%1 specifications received").arg(specificationResponse.tasks().length()));

    foreach (const ScheduleDefinition &definition, specificationResponse.tasks())
    {
        scheduler->enqueue(definition);
    }
}


MPlaneController::MPlaneController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{

}

MPlaneController::~MPlaneController()
{
    delete d;
}

bool MPlaneController::init(NetworkManager *networkManager, Scheduler *scheduler, Settings *settings)
{
    d->networkManager = networkManager;
    d->scheduler = scheduler;
    d->settings = settings;

    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(updateTimer()));
    d->updateTimer();

    return true;
}

Controller::Status MPlaneController::status() const
{
    return Controller::Unknown;
}

QString MPlaneController::errorString() const
{
    return "";
}

void MPlaneController::sendCapabilities()
{
    d->postRequest.setPath("/register/capability");
    d->postRequest.addData(PingDefinition::capability());
    d->postRequest.addData(HTTPDownloadDefinition::capability());
    d->postRequest.addData(DnslookupDefinition::capability());
    d->postRequest.addData(TracerouteDefinition::capability());
    d->postRequest.addData(ReverseDnslookupDefinition::capability());
    d->postRequest.setAuthenticationMethod(Request::None);
    d->capabilityRequester.setRequest(&d->postRequest);
    d->capabilityRequester.setResponse(&d->capabilityResponse);

    d->capabilityRequester.start();
}

void MPlaneController::fetchSpecifications()
{
    d->getRequest.setPath("/show/specification");
    d->getRequest.setAuthenticationMethod(Request::None);
    d->specificationRequester.setRequest(&d->getRequest);
    d->specificationRequester.setResponse(&d->specificationResponse);

    d->specificationRequester.start();
}

#include "mplanecontroller.moc"
