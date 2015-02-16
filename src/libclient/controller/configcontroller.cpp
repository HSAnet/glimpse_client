#include "configcontroller.h"
#include "../network/networkmanager.h"
#include "../settings.h"
#include "../log/logger.h"

#include "../webrequester.h"
#include "../network/requests/getconfigrequest.h"
#include "../timing/periodictiming.h"
#include "../client.h"
#include "../timing/timer.h"

#include <QPointer>

LOGGER(ConfigController);

class ConfigController::Private : public QObject
{
    Q_OBJECT

public:
    Private(ConfigController *q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), q, SLOT(update()));
        connect(&timer, SIGNAL(timingChanged()), this, SLOT(onTimingChanged()));
        connect(&requester, SIGNAL(statusChanged(WebRequester::Status)), q, SIGNAL(statusChanged()));
        connect(&requester, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(&requester, SIGNAL(error()), this, SLOT(onError()));

        request.setPath("/supervisor/api/v1/configuration/1/");
        requester.setRequest(&request);
    }

    ConfigController *q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    WebRequester requester;
    GetConfigRequest request;
    GetConfigResponse *response;

    Timer timer;

    // Functions
public slots:
    void updateTimer();
    void onFinished();
    void onError();
    void onTimingChanged();
};

void ConfigController::Private::updateTimer()
{
    // Set the new url
    QString newUrl = QString("https://%1").arg(settings->config()->configAddress());

    if (requester.url() != newUrl)
    {
        LOG_DEBUG(QString("Config url set to %1").arg(newUrl));
        requester.setUrl(newUrl);
    }

    TimingPtr timing = settings->config()->configTiming();

    if (timing.isNull() || !timing->nextRun().isValid())
    {
        timing = TimingPtr(new PeriodicTiming(10*60*1000, QDateTime(), QDateTime(), 30*1000));
    }

    timer.setTiming(timing);
    timer.start();
}

void ConfigController::Private::onFinished()
{
    emit q->finished();
}

void ConfigController::Private::onError()
{
    LOG_ERROR(QString("Could not get config from server, trying again later"));
}

void ConfigController::Private::onTimingChanged()
{
    LOG_DEBUG(QString("Config schedule changed, type: %1, nextRun in %2 seconds.")
              .arg(timer.timing()->type())
              .arg(timer.timing()->timeLeft() / 1000));
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

    // TODO: This is an evil hack
    d->response = settings->config();
    d->requester.setResponse(d->response);
    connect(d->response, SIGNAL(responseChanged()), d, SLOT(updateTimer()));

    // sets the url into the requester and starts the timer
    d->updateTimer();

    // get new config now
    update();

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
