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
    QString mail;
    QString firstName;
    QString password;
    int xpLevel;
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
    return d->mail;
}

void UserRegisterRequest::setUserId(const QString &mail)
{
    if (d->mail != mail)
    {
        d->mail = mail;
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

void UserRegisterRequest::setXpLevel(int xpLevel)
{
    if (d->xpLevel != xpLevel)
    {
        d->xpLevel = xpLevel;
        emit xpLevelChanged(xpLevel);
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
    return data;
}
