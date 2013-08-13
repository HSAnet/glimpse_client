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
    virtual void finished();

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
    void finished();

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

class TimingInformation : public Response
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type NOTIFY responseChanged)
    Q_PROPERTY(int interval READ interval NOTIFY responseChanged)

public:
    TimingInformation(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);

    QString type() const;
    int interval() const;

signals:
    void responseChanged();

protected:
    QString m_type;
    int m_interval;
};

class GetConfigResponse : public Response
{
    Q_OBJECT
    Q_PROPERTY(QString controllerAddress READ controllerAddress NOTIFY responseChanged)
    Q_PROPERTY(QString keepaliveAddress READ keepaliveAddress NOTIFY responseChanged)
    Q_PROPERTY(TimingInformation* fetchTaskSchedule READ fetchTaskSchedule NOTIFY responseChanged)
    Q_PROPERTY(TimingInformation* keepaliveSchedule READ keepaliveSchedule NOTIFY responseChanged)
    Q_PROPERTY(TimingInformation* updateConfigSchedule READ updateConfigSchedule NOTIFY responseChanged)

public:
    GetConfigResponse(QObject* parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    void finished();

    QString controllerAddress() const;
    TimingInformation *fetchTaskSchedule() const;

    QString keepaliveAddress() const;
    TimingInformation* keepaliveSchedule() const;
    TimingInformation* updateConfigSchedule() const;

signals:
    void responseChanged();

protected:
    QString m_controllerAddress;
    TimingInformation* m_fetchTaskSchedule;
    QString m_keepaliveAddress;
    TimingInformation* m_keepaliveSchedule;
    TimingInformation* m_updateConfigSchedule;
};

#endif // RESPONSE_H
