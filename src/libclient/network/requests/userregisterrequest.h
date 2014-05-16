#ifndef USERREGISTERREQUEST_H
#define USERREGISTERREQUEST_H

#include "request.h"

#include <QLocale>

class CLIENT_API UserRegisterRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/auth/api/v1/user/")
    Q_CLASSINFO("http_request_method", "post")
    Q_PROPERTY(QString nameLast READ nameLast WRITE setNameLast NOTIFY nameLastChanged)
    Q_PROPERTY(QLocale::Country homeCountry READ homeCountry WRITE setHomeCountry NOTIFY homeCountryChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY mailChanged)
    Q_PROPERTY(QString nameFirst READ nameFirst WRITE setNameFirst NOTIFY nameFirstChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(int xpLevel READ xpLevel WRITE setXpLevel NOTIFY xpLevelChanged)

public:
    UserRegisterRequest(QObject* parent = 0);
    ~UserRegisterRequest();

    QVariant toVariant() const;

    QString nameLast() const;
    QLocale::Country homeCountry() const;
    QString userId() const;
    QString nameFirst() const;
    QString password() const;
    int xpLevel() const;

    void setNameLast(const QString& nameLast);
    void setHomeCountry(QLocale::Country homeCountry);
    void setUserId(const QString& userId);
    void setNameFirst(const QString& nameFirst);
    void setPassword(const QString& password);
    void setXpLevel(int xpLevel);

signals:
    void nameLastChanged(const QString& nameLast);
    void homeCountryChanged(QLocale::Country homeCountry);
    void mailChanged(const QString& userId);
    void nameFirstChanged(const QString& nameFirst);
    void passwordChanged(const QString& password);
    void xpLevelChanged(int xpLevel);

protected:
    class Private;
    Private* d;
};

#endif // USERREGISTERREQUEST_H
