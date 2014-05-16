#ifndef LOGINREQUEST_H
#define LOGINREQUEST_H

#include "request.h"

class CLIENT_API LoginRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/auth/api/v1/apikey/1/")
    Q_CLASSINFO("http_request_method", "get")
    Q_CLASSINFO("authentication_method", "basic")
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)

public:
    LoginRequest(QObject* parent = 0);
    ~LoginRequest();

    QVariant toVariant() const;

    QString password() const;
    QString userId() const;

    void setPassword(const QString& password);
    void setUserId(const QString& userId);

signals:
    void passwordChanged(const QString& password);
    void userIdChanged(const QString& userId);

protected:
    class Private;
    Private* d;
};

#endif // LOGINREQUEST_H
