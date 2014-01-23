#ifndef RESPONSE_H
#define RESPONSE_H

#include <QObject>
#include <QVariantMap>

#include "timing/timing.h"

// TODO: Remove QObject base if we don't use it anymore in DeviceRegistration.qml

class CLIENT_API Response : public QObject
{
    Q_OBJECT

public:
    explicit Response(QObject *parent = 0);
    ~Response();

    virtual bool fillFromVariant(const QVariantMap& variant) = 0;
    virtual void finished();
};

class CLIENT_API LoginResponse : public Response
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

class CLIENT_API RegisterDeviceResponse : public Response
{
    Q_OBJECT

public:
    RegisterDeviceResponse(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
};

class CLIENT_API GetConfigResponse : public Response
{
    Q_OBJECT

public:
    GetConfigResponse(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    QVariant toVariant() const;
    void finished();

    void setControllerAddress(const QString& controllerAddress);
    QString controllerAddress() const;
    TimingPtr fetchTaskSchedule() const;

    QString keepaliveAddress() const;
    TimingPtr keepaliveSchedule() const;
    TimingPtr updateConfigSchedule() const;

    TimingPtr reportSchedule() const;

signals:
    void responseChanged();

protected:
    QString m_controllerAddress;
    TimingPtr m_fetchTaskSchedule;
    QString m_keepaliveAddress;
    TimingPtr m_keepaliveSchedule;
    TimingPtr m_updateConfigSchedule;
    TimingPtr m_reportSchedule;
};

#endif // RESPONSE_H
