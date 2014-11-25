#ifndef USERREGISTERREQUEST_H
#define USERREGISTERREQUEST_H

#include "request.h"

#include <QLocale>

class CLIENT_API UserRegisterRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("http_request_method", "post")
    Q_CLASSINFO("authentication_method", "none")
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY lastNameChanged)
    Q_PROPERTY(QLocale::Country homeCountry READ homeCountry WRITE setHomeCountry NOTIFY homeCountryChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY mailChanged)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY firstNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(int xpLevel READ xpLevel WRITE setXpLevel NOTIFY xpLevelChanged)
    Q_PROPERTY(QString eMail READ eMail WRITE setEmail NOTIFY eMailChanged)

public:
    UserRegisterRequest(QObject *parent = 0);
    ~UserRegisterRequest();

    QVariant toVariant() const;

    QString lastName() const;
    QLocale::Country homeCountry() const;
    QString userId() const;
    QString firstName() const;
    QString password() const;
    int xpLevel() const;
    QString eMail() const;

    void setLastName(const QString &lastName);
    void setHomeCountry(QLocale::Country homeCountry);
    void setUserId(const QString &userId);
    void setFirstName(const QString &firstName);
    void setPassword(const QString &password);
    void setXpLevel(int xpLevel);
    void setEmail(const QString &eMail);

signals:
    void lastNameChanged(const QString &lastName);
    void homeCountryChanged(QLocale::Country homeCountry);
    void mailChanged(const QString &userId);
    void firstNameChanged(const QString &firstName);
    void passwordChanged(const QString &password);
    void xpLevelChanged(int xpLevel);
    void eMailChanged(const QString &eMail);

protected:
    class Private;
    Private *d;
};

#endif // USERREGISTERREQUEST_H
