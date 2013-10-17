#include "logincontroller.h"
#include "../network/networkmanager.h"
#include "../settings.h"

#include "../webrequester.h"
#include "../network/requests/loginrequest.h"
#include "../response.h"

#include <QPointer>

class LoginController::Private
{
public:
    Private()
    {
        requester.setRequest(&request);
        requester.setResponse(&response);
    }

    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    WebRequester requester;
    LoginRequest request;
    LoginResponse response;
};

LoginController::LoginController(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

LoginController::~LoginController()
{
    delete d;
}

bool LoginController::isLoggedIn() const
{
    return false;
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
