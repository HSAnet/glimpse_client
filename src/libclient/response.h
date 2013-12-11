#ifndef RESPONSE_H
#define RESPONSE_H

#include <QObject>
#include <QVariantMap>

// TODO: Remove QObject base if we don't use it anymore in DeviceRegistration.qml

class Response : public QObject
{
    Q_OBJECT

public:
    explicit Response(QObject *parent = 0);
    ~Response();

    virtual bool fillFromVariant(const QVariantMap& variant) = 0;
    virtual void finished();
};

class LoginResponse : public Response
{
    Q_OBJECT

public:
    LoginResponse(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    void finished();

    QString sessionId() const;
    bool registeredDevice() const;

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

class TimingInformation : public Response
{
    Q_OBJECT

public:
    TimingInformation(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    QVariant toVariant() const;

    QString type() const;
    int interval() const;

protected:
    QString m_type;
    int m_interval;
};

class GetConfigResponse : public Response
{
    Q_OBJECT

public:
    GetConfigResponse(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    QVariant toVariant() const;
    void finished();

    void setControllerAddress(const QString& controllerAddress);
    QString controllerAddress() const;
    TimingInformation *fetchTaskSchedule() const;

    QString keepaliveAddress() const;
    TimingInformation* keepaliveSchedule() const;
    TimingInformation* updateConfigSchedule() const;

protected:
    QString m_controllerAddress;
    TimingInformation* m_fetchTaskSchedule;
    QString m_keepaliveAddress;
    TimingInformation* m_keepaliveSchedule;
    TimingInformation* m_updateConfigSchedule;
};

#endif // RESPONSE_H
