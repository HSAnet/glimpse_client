#include "logincontroller.h"
#include "../network/networkmanager.h"
#include "../settings.h"
#include "../log/logger.h"

#include "../webrequester.h"
#include "../network/requests/loginrequest.h"
#include "../network/requests/userregisterrequest.h"
#include "../response.h"

#include <QPointer>

LOGGER(LoginController);

class LoginController::Private : public QObject
{
    Q_OBJECT

public:
    Private(LoginController* q)
    : q(q)
    , loggedIn(false)
    , registeredDevice(false)
    {
        connect(&requester, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(&requester, SIGNAL(error()), this, SLOT(onError()));
        connect(&requester, SIGNAL(statusChanged(Status)), q, SIGNAL(statusChanged()));

        requester.setResponse(&response);
    }

    LoginController* q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    WebRequester requester;

    UserRegisterRequest registerRequest;
    LoginRequest loginRequest;
    LoginResponse response;

    bool loggedIn;
    bool registeredDevice;

    // Functions
    void setLoggedIn(bool loggedIn);
    void setRegisterdDevice(bool registeredDevice);

public slots:
    void onFinished();
    void onError();
};

void LoginController::Private::setLoggedIn(bool loggedIn)
{
    if (this->loggedIn != loggedIn) {
        this->loggedIn = loggedIn;
        emit q->loggedInChanged();
    }
}

void LoginController::Private::setRegisterdDevice(bool registeredDevice)
{
    if (this->registeredDevice != registeredDevice) {
        this->registeredDevice = registeredDevice;
        emit q->registeredDeviceChanged();
    }
}

void LoginController::Private::onFinished()
{
    LOG_INFO("Login/Registration successful");

    // After registration we save the login data
    if (requester.request() == &registerRequest) {
        settings->setUserId(registerRequest.userId());
        settings->setPassword(registerRequest.password());

        LOG_DEBUG("Wrote username and password to settings");
    }

    settings->setSessionId(response.sessionId());

    setRegisterdDevice(response.registeredDevice());
    setLoggedIn(true);
}

void LoginController::Private::onError()
{
    LOG_INFO(QString("Login/Registration failed: %1").arg(requester.errorString()));
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
    QUuid uuid = QUuid::createUuid();
    QString username = QString("%1@%2.anon").arg(uuid.toString()).arg(uuid.toString());
    QString password = QUuid::createUuid().createUuid().toString();

    LOG_INFO("Anonymous registration requested. Scrambled some data.");

    registration(username, password);
}

void LoginController::registration(const QString &username, const QString &password)
{
    d->registerRequest.setUserId(username);
    d->registerRequest.setPassword(password);

    d->requester.setRequest(&d->registerRequest);
    d->requester.start();
}

void LoginController::login()
{
    d->loginRequest.setUserId(d->settings->userId());
    d->loginRequest.setPassword(d->settings->password());

    d->requester.setRequest(&d->loginRequest);
    d->requester.start();
}

bool LoginController::init(NetworkManager *networkManager, Settings *settings)
{
    d->networkManager = networkManager;
    d->settings = settings;
    return true;
}

#include "logincontroller.moc"
