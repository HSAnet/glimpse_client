#include "logincontroller.h"
#include "../network/networkmanager.h"
#include "../settings.h"
#include "../log/logger.h"

#include "../webrequester.h"
#include "../network/requests/loginrequest.h"
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

        requester.setRequest(&request);
        requester.setResponse(&response);
    }

    LoginController* q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    WebRequester requester;
    LoginRequest request;
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
    LOG_INFO("Login successful");
    setRegisterdDevice(response.registeredDevice());
    setLoggedIn(true);
}

void LoginController::Private::onError()
{
    LOG_INFO(QString("Login failed: %1").arg(requester.errorString()));
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

void LoginController::login()
{
    d->request.setUserId(d->settings->userId());
    d->request.setPassword(d->settings->password());

    d->requester.start();
}

bool LoginController::init(NetworkManager *networkManager, Settings *settings)
{
    d->networkManager = networkManager;
    d->settings = settings;
    return true;
}

#include "logincontroller.moc"
