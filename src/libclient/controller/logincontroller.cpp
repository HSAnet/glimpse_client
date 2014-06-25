#include "logincontroller.h"
#include "../network/networkmanager.h"
#include "../settings.h"
#include "../log/logger.h"

#include "../webrequester.h"
#include "../network/requests/loginrequest.h"
#include "../network/requests/userregisterrequest.h"
#include "../network/requests/resourcerequest.h"
#include "../response.h"
#include "../client.h"
#include "../types.h"

#include <QPointer>

LOGGER(LoginController);

class LoginController::Private : public QObject
{
    Q_OBJECT

public:
    Private(LoginController *q)
    : q(q)
    , loggedIn(false)
    , registeredDevice(false)
    {
        connect(&requester, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(&requester, SIGNAL(error()), this, SLOT(onError()));
        connect(&requester, SIGNAL(statusChanged(WebRequester::Status)), q, SIGNAL(statusChanged()));
        connect(&requester, SIGNAL(started()), q, SIGNAL(started()));

        taskRequest.setPath(("/supervisor/api/v1/instruction/1/"));
        requester.setResponse(&response);
    }

    LoginController *q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    WebRequester requester;

    UserRegisterRequest registerRequest;
    LoginRequest loginRequest;
    LoginResponse response;
    GetResourceRequest taskRequest;
    GetInstructionResponse taskResponse;

    bool loggedIn;
    bool registeredDevice;

    // Functions
    void setLoggedIn(bool loggedIn);
    void setRegisterdDevice(bool registeredDevice);

public slots:
    void updateController();
    void onFinished();
    void onError();
};

void LoginController::Private::setLoggedIn(bool loggedIn)
{
    if (this->loggedIn != loggedIn)
    {
        this->loggedIn = loggedIn;
        emit q->loggedInChanged();
    }
}

void LoginController::Private::setRegisterdDevice(bool registeredDevice)
{
    if (this->registeredDevice != registeredDevice)
    {
        this->registeredDevice = registeredDevice;
        emit q->registeredDeviceChanged();
    }
}

void LoginController::Private::updateController()
{
    // Set the new url, we use the config address here at the moment
    QString newUrl = QString("http://%1").arg(Client::instance()->settings()->config()->supervisorAddress());

    if (requester.url() != newUrl)
    {
        LOG_INFO(QString("Login url set to %1").arg(newUrl));
        requester.setUrl(newUrl);
    }
}

void LoginController::Private::onFinished()
{
    bool isLogin = true;

    LOG_INFO("Login/Registration successful");

    // After registration we save the login data
    if (requester.request() == &registerRequest)
    {
        isLogin = false;
        settings->setUserId(registerRequest.userId());
        LOG_DEBUG("Wrote username to settings");
    }

    setRegisterdDevice(response.registeredDevice());
    setLoggedIn(true);

    emit q->finished();

    if (isLogin)
    {
        emit q->loginFinished();
    }
    else
    {
        emit q->registrationFinished();
    }
}

void LoginController::Private::onError()
{
    LOG_INFO(QString("Login/Registration failed: %1").arg(requester.errorString()));
    emit q->error();
}

LoginController::LoginController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

LoginController::~LoginController()
{
    delete d;
}

LoginController::Status LoginController::status() const
{
    return (Status)d->requester.status();
}

bool LoginController::isLoggedIn() const
{
    return d->loggedIn;
}

bool LoginController::registeredDevice() const
{
    return d->registeredDevice;
}

QString LoginController::errorString() const
{
    return d->requester.errorString();
}

void LoginController::anonymousRegistration()
{
    QString username = QString("%1@anon.com").arg(uuidToString(QUuid::createUuid()).left(
                                                      15)); // TODO increase username length in database
    QString password = uuidToString(QUuid::createUuid()).left(15);

    LOG_INFO("Anonymous registration requested. Scrambled some data.");

    registration(username, password);
}

void LoginController::registration(const QString &username, const QString &password)
{
    d->registerRequest.setUserId(username);
    d->registerRequest.setPassword(password);

    d->requester.setRequest(&d->registerRequest);
    d->requester.setResponse(&d->response);
    d->requester.start();
}

void LoginController::login()
{
    d->loginRequest.setUserId(d->settings->userId());
    d->loginRequest.setPassword(d->settings->password());

    d->requester.setRequest(&d->loginRequest);
    d->requester.setResponse(&d->response);
    d->requester.start();
}

void LoginController::checkApiKey()
{

    d->requester.setRequest(&d->taskRequest);
    d->requester.setResponse(&d->taskResponse);
    d->requester.start();
}

bool LoginController::init(NetworkManager *networkManager, Settings *settings)
{
    d->networkManager = networkManager;
    d->settings = settings;

    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(updateController()));
    d->updateController();

    return true;
}

#include "logincontroller.moc"
