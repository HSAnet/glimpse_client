#include "userregisterrequest.h"

class UserRegisterRequest::Private
{
public:
    Private()
    : homeCountry(QLocale::system().country())
    , xpLevel(0)
    {
    }

    QString lastName;
    QLocale::Country homeCountry;
    QString userId;
    QString firstName;
    QString password;
    int xpLevel;
    QString eMail;
};

UserRegisterRequest::UserRegisterRequest(QObject *parent)
: Request(parent)
, d(new Private)
{
    setPath("/auth/api/v1/user/");
}

UserRegisterRequest::~UserRegisterRequest()
{
    delete d;
}

QString UserRegisterRequest::lastName() const
{
    return d->lastName;
}

void UserRegisterRequest::setLastName(const QString &lastName)
{
    if (d->lastName != lastName)
    {
        d->lastName = lastName;
        emit lastNameChanged(lastName);
    }
}

QLocale::Country UserRegisterRequest::homeCountry() const
{
    return d->homeCountry;
}

void UserRegisterRequest::setHomeCountry(QLocale::Country homeCountry)
{
    if (d->homeCountry != homeCountry)
    {
        d->homeCountry = homeCountry;
        emit homeCountryChanged(homeCountry);
    }
}

QString UserRegisterRequest::userId() const
{
    return d->userId;
}

void UserRegisterRequest::setUserId(const QString &mail)
{
    if (d->userId != mail)
    {
        d->userId = mail;
        emit mailChanged(mail);
    }
}

QString UserRegisterRequest::firstName() const
{
    return d->firstName;
}

void UserRegisterRequest::setFirstName(const QString &firstName)
{
    if (d->firstName != firstName)
    {
        d->firstName = firstName;
        emit firstNameChanged(firstName);
    }
}

QString UserRegisterRequest::password() const
{
    return d->password;
}

void UserRegisterRequest::setPassword(const QString &password)
{
    if (d->password != password)
    {
        d->password = password;
        emit passwordChanged(password);
    }
}

int UserRegisterRequest::xpLevel() const
{
    return d->xpLevel;
}

QString UserRegisterRequest::eMail() const
{
    return d->eMail;
}

void UserRegisterRequest::setXpLevel(int xpLevel)
{
    if (d->xpLevel != xpLevel)
    {
        d->xpLevel = xpLevel;
        emit xpLevelChanged(xpLevel);
    }
}

void UserRegisterRequest::setEmail(const QString &eMail)
{
    if (d->eMail != eMail)
    {
        d->eMail = eMail;
        emit eMailChanged(eMail);
    }
}

QVariant UserRegisterRequest::toVariant() const
{
    QVariantMap data;
    data.insert("last_name", lastName());
    data.insert("home_country", QLocale::system().bcp47Name());
    data.insert("username", userId());
    data.insert("first_name", firstName());
    data.insert("password", password());
    data.insert("xp_level", xpLevel());
    data.insert("email", eMail());
    return data;
}
