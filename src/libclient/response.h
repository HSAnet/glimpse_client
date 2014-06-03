#ifndef RESPONSE_H
#define RESPONSE_H

#include <QObject>
#include <QVariantMap>

#include "timing/timing.h"
#include "channel.h"

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
    RegisterDeviceResponse(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
};

class CLIENT_API GetConfigResponse : public Response
{
    Q_OBJECT
    Q_PROPERTY(QString supervisorAddress READ supervisorAddress CONSTANT)

public:
    GetConfigResponse(QObject* parent = 0);

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
    ChannelPtr m_supervisorChannel;
    ChannelPtr m_keepaliveChannel;
    ChannelPtr m_configChannel;
    ChannelPtr m_reportChannel;
};

#endif // RESPONSE_H
