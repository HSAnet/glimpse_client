#include "loginrequest.h"
#include "localinformation.h"

class LoginRequest::Private
{
public:
    QString password;
    QString userId;
    LocalInformation localInformation;
};

LoginRequest::LoginRequest(QObject *parent)
: Request(parent)
, d(new Private)
{
    setPath("/auth/api/v1/login/1/");
}

LoginRequest::~LoginRequest()
{
    delete d;
}

QString LoginRequest::password() const
{
    return d->password;
}

void LoginRequest::setPassword(const QString &password)
{
    if (d->password != password)
    {
        d->password = password;
        emit passwordChanged(password);
    }
}

QString LoginRequest::userId() const
{
    return d->userId;
}

void LoginRequest::setUserId(const QString &userId)
{
    if (d->userId != userId)
    {
        d->userId = userId;
        emit userIdChanged(userId);
    }
}

QVariant LoginRequest::toVariant() const
{
    QVariantMap data = d->localInformation.getConstants();
    return data;
}
