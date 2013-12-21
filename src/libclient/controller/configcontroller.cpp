#include "configcontroller.h"
#include "../network/networkmanager.h"
#include "../settings.h"
#include "../log/logger.h"

#include "../webrequester.h"
#include "../network/requests/getconfigrequest.h"
#include "../response.h"
#include "../timing/periodictiming.h"

#include <QPointer>
#include <QTimer>

LOGGER(ConfigController);

class ConfigController::Private : public QObject
{
    Q_OBJECT

public:
    Private(ConfigController* q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), q, SLOT(update()));
        connect(&requester, SIGNAL(statusChanged(Status)), q, SIGNAL(statusChanged()));
        connect(&requester, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(&requester, SIGNAL(error()), this, SLOT(onError()));

        requester.setRequest(&request);
        //requester.setResponse(&response);
    }

    ConfigController* q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    WebRequester requester;
    GetConfigRequest request;
    //GetConfigResponse response;
    GetConfigResponse* response;

    QTimer timer;

    // Functions
public slots:
    void updateTimer();
    void onFinished();
    void onError();
};

void ConfigController::Private::updateTimer()
{
    TimingPtr timing = settings->config()->updateConfigSchedule();
    if (timing.isNull()) {
        timer.stop();
        return;
    }

    QSharedPointer<PeriodicTiming> periodicTiming = timing.dynamicCast<PeriodicTiming>();
    Q_ASSERT(periodicTiming);

    int period = periodicTiming->period();
    timer.setInterval(period);
    timer.start();

    LOG_INFO(QString("Update Config schedule set to %1 sec.").arg(period/1000));
}

void ConfigController::Private::onFinished()
{
}

void ConfigController::Private::onError()
{
}

ConfigController::ConfigController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

ConfigController::~ConfigController()
{
    delete d;
}

void ConfigController::update()
{
    d->requester.start();
}

bool ConfigController::init(NetworkManager *networkManager, Settings *settings)
{
    d->networkManager = networkManager;
    d->settings = settings;

    // FIXME: This is a evil hack
    d->response = settings->config();
    d->requester.setResponse(d->response);
    connect(d->response, SIGNAL(responseChanged()), d, SLOT(updateTimer()));

    d->updateTimer();

    return true;
}

Controller::Status ConfigController::status() const
{
    return (Status)d->requester.status();
}

QString ConfigController::errorString() const
{
    return d->requester.errorString();
}

#include "configcontroller.moc"
