#include "configcontroller.h"
#include "../network/networkmanager.h"
#include "../settings.h"
#include "../log/logger.h"

#include "../webrequester.h"
#include "../network/requests/getconfigrequest.h"
#include "../response.h"

#include <QPointer>

LOGGER(ConfigController);

class ConfigController::Private : public QObject
{
    Q_OBJECT

public:
    Private(ConfigController* q)
    : q(q)
    {
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

    // Functions
public slots:
    void onFinished();
    void onError();
};

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
    d->requester.setResponse(settings->config());

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
