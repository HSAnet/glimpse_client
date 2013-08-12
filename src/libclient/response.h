#ifndef RESPONSE_H
#define RESPONSE_H

#include <QObject>
#include <QVariantMap>

class Response : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorText READ errorText NOTIFY errorTextChanged)

public:
    explicit Response(QObject *parent = 0);
    ~Response();

    QString errorText() const;

    virtual bool fillFromVariant(const QVariantMap& variant) = 0;

signals:
    void errorTextChanged(const QString& errorText);

protected:
    QString m_errorText;
};

class LoginResponse : public Response
{
    Q_OBJECT
    Q_PROPERTY(QString sessionId READ sessionId NOTIFY responseChanged)
    Q_PROPERTY(bool registeredDevice READ registeredDevice NOTIFY responseChanged)

public:
    LoginResponse(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);

    QString sessionId() const;
    bool registeredDevice() const;

signals:
    void responseChanged();

protected:
    QString m_sessionId;
    bool m_registeredDevice;
};

typedef LoginResponse RegisterUserResponse;

class RegisterDeviceResponse : public Response
{
    Q_OBJECT

public:
    RegisterDeviceResponse(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
};

#endif // RESPONSE_H
