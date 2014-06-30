#ifndef RESPONSE_H
#define RESPONSE_H

#include <QObject>
#include <QVariantMap>

#include "timing/timing.h"
#include "channel.h"
#include "task/task.h"
#include "task/taskvalidator.h"

// TODO: Remove QObject base if we don't use it anymore in DeviceRegistration.qml

class CLIENT_API Response : public QObject
{
    Q_OBJECT

public:
    explicit Response(QObject *parent = 0);
    ~Response();

    virtual bool fillFromVariant(const QVariantMap &variant) = 0;
    virtual void finished();
};

class CLIENT_API LoginResponse : public Response
{
    Q_OBJECT

public:
    LoginResponse(QObject *parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    void finished();

    QString apiKey() const;
    bool registeredDevice() const;

protected:
    QString m_apiKey;
    bool m_registeredDevice;
};

typedef LoginResponse RegisterUserResponse;

class CLIENT_API RegisterDeviceResponse : public Response
{
    Q_OBJECT

public:
    RegisterDeviceResponse(QObject *parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
};

class CLIENT_API GetConfigResponse : public Response
{
    Q_OBJECT
    Q_PROPERTY(QString supervisorAddress READ supervisorAddress CONSTANT)

public:
    GetConfigResponse(QObject *parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    QVariant toVariant() const;
    void finished();

    QString supervisorAddress() const;
    TimingPtr supervisorTiming() const;
    QString keepaliveAddress() const;
    TimingPtr keepaliveTiming() const;
    void setConfigAddress(const QString &address);
    QString configAddress() const;
    TimingPtr configTiming() const;
    QString reportAddress() const;
    TimingPtr reportTiming() const;


signals:
    void responseChanged();

protected:
    Channel m_supervisorChannel;
    Channel m_keepaliveChannel;
    Channel m_configChannel;
    Channel m_reportChannel;
};

class GetInstructionResponse : public Response
{
    Q_OBJECT

public:
    GetInstructionResponse(QObject *parent = 0)
    : Response(parent)
    {
    }

    QList<int> taskIds() const;
    QList<int> scheduleIds() const;
    bool fillFromVariant(const QVariantMap &variant);

protected:
    QList<int> m_taskIds;
    QList<int> m_scheduleIds;
};

class GetScheduleResponse : public Response
{
    Q_OBJECT

public:
    GetScheduleResponse(QObject *parent = 0)
    : Response(parent)
    {
    }

    bool fillFromVariant(const QVariantMap &variant);
    TestDefinitionList tasks() const;

protected:
    TaskValidator m_validator;
    TestDefinitionList m_tasks;
};

#endif // RESPONSE_H
