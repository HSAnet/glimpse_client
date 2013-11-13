#include "registrationcontroller.h"
#include "../settings.h"
#include "../webrequester.h"
#include "../response.h"
#include "../network/requests/userregisterrequest.h"
#include "../network/networkmanager.h"
#include "../log/logger.h"

#include "../client.h"

#include <QPointer>

LOGGER(RegistrationController);

class RegistrationController::Private
{
public:
    Private(RegistrationController* q)
    : q(q)
    {
        connect(&requester, SIGNAL(statusChanged(Status)), q, SIGNAL(statusChanged()));

        requester.setRequest(&request);
        requester.setResponse(&response);
    }

    RegistrationController* q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    UserRegisterRequest request;
    RegisterUserResponse response;
    WebRequester requester;

    bool isRegistered;

    // Functions
    void setRegistered(bool registered);

public slots:
    void onFinished();
    void onError();
};

void RegistrationController::Private::setRegistered(bool registered)
{
    if (this->isRegistered != registered) {
        this->isRegistered = registered;
        emit q->registeredChanged();
    }
}

void RegistrationController::Private::onFinished()
{
    LOG_INFO("Registration successful.");

    // Update session id and registration status
    settings->setSessionId(response.sessionId());

    Client* client = Client::instance();
    client->setStatus(Client::Registered);
}

void RegistrationController::Private::onError()
{
    LOG_INFO(QString("Registration failed: %1").arg(requester.errorString()));
}

RegistrationController::RegistrationController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

RegistrationController::~RegistrationController()
{
    delete d;
}

bool RegistrationController::isRegistered() const
{
    return d->isRegistered;
}

void RegistrationController::anonymousRegistration()
{
    QUuid uuid = QUuid::createUuid();
    QString username = QString("%1@%2.anon").arg(uuid.toString()).arg(uuid.toString());
    QString password = QUuid::createUuid().createUuid().toString();

    LOG_INFO("Anonymous registration requested. Scrambled some data.");

    registration(username, password);
}

void RegistrationController::registration(const QString &username, const QString &password)
{

}

Controller::Status RegistrationController::status() const
{
    return (Status)d->requester.status();
}

QString RegistrationController::errorString() const
{
    return d->requester.errorString();
}

bool RegistrationController::init(NetworkManager *networkManager, Settings *settings)
{
    d->networkManager = networkManager;
    d->settings = settings;
    return true;
}
