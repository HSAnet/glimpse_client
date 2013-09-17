#include "userregisterrequest.h"

class UserRegisterRequest::Private
{
public:
    Private()
    : homeCountry(QLocale::system().country())
    , xpLevel(0)
    {
    }

    QString nameLast;
    QLocale::Country homeCountry;
    QString mail;
    QString nameFirst;
    QString password;
    int xpLevel;
};

UserRegisterRequest::UserRegisterRequest(QObject *parent)
: Request(parent)
, d(new Private)
{
}

UserRegisterRequest::~UserRegisterRequest()
{
    delete d;
}

QString UserRegisterRequest::nameLast() const {
  return d->nameLast;
}

void UserRegisterRequest::setNameLast(const QString& nameLast) {
  if (d->nameLast != nameLast) {
    d->nameLast = nameLast;
    emit nameLastChanged(nameLast);
  }
}

QLocale::Country UserRegisterRequest::homeCountry() const {
  return d->homeCountry;
}

void UserRegisterRequest::setHomeCountry(QLocale::Country homeCountry) {
  if (d->homeCountry != homeCountry) {
    d->homeCountry = homeCountry;
    emit homeCountryChanged(homeCountry);
  }
}

QString UserRegisterRequest::userId() const {
  return d->mail;
}

void UserRegisterRequest::setUserId(const QString& mail) {
  if (d->mail != mail) {
    d->mail = mail;
    emit mailChanged(mail);
  }
}

QString UserRegisterRequest::nameFirst() const {
  return d->nameFirst;
}

void UserRegisterRequest::setNameFirst(const QString& nameFirst) {
  if (d->nameFirst != nameFirst) {
    d->nameFirst = nameFirst;
    emit nameFirstChanged(nameFirst);
  }
}

QString UserRegisterRequest::password() const {
  return d->password;
}

void UserRegisterRequest::setPassword(const QString& password) {
  if (d->password != password) {
    d->password = password;
    emit passwordChanged(password);
  }
}

int UserRegisterRequest::xpLevel() const {
  return d->xpLevel;
}

void UserRegisterRequest::setXpLevel(int xpLevel) {
  if (d->xpLevel != xpLevel) {
    d->xpLevel = xpLevel;
    emit xpLevelChanged(xpLevel);
  }
}

QVariant UserRegisterRequest::toVariant() const {
  QVariantMap data;
  data.insert("name_last", nameLast());
  data.insert("home_country", QLocale::system().bcp47Name());
  data.insert("user_id", userId());
  data.insert("name_first", nameFirst());
  data.insert("password", password());
  data.insert("xp_level", xpLevel());
  return data;
}
