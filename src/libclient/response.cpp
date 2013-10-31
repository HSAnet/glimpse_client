#include "response.h"
#include "client.h"
#include "settings.h"

Response::Response(QObject *parent)
: QObject(parent)
{
}

Response::~Response()
{
}

QString Response::errorText() const
{
    return m_errorText;
}

void Response::finished()
{
}

LoginResponse::LoginResponse(QObject *parent)
: Response(parent)
{
}

bool LoginResponse::fillFromVariant(const QVariantMap &variant)
{
    m_sessionId = variant.value("session_id").toString();
    m_registeredDevice = variant.value("registered_device", true).toBool();

    emit responseChanged();

    return !m_sessionId.isEmpty();
}

void LoginResponse::finished()
{
    Client* client = Client::instance();

    // Update the settings
    Settings* settings = client->settings();
    settings->setSessionId(m_sessionId);

    // Set registered status (after settings!)
    client->setStatus(Client::Registered);
}

QString LoginResponse::sessionId() const
{
    return m_sessionId;
}

bool LoginResponse::registeredDevice() const
{
    return m_registeredDevice;
}


RegisterDeviceResponse::RegisterDeviceResponse(QObject *parent)
: Response(parent)
{
}

bool RegisterDeviceResponse::fillFromVariant(const QVariantMap &variant)
{
    Q_UNUSED(variant);
    return true;
}


TimingInformation::TimingInformation(QObject *parent)
: Response(parent)
, m_interval(0)
{
}

bool TimingInformation::fillFromVariant(const QVariantMap &variant)
{
    m_type = variant.value("type").toString();
    m_interval = variant.value("interval").toInt();

    emit responseChanged();
    return true;
}

QVariant TimingInformation::toVariant() const
{
    QVariantMap map;
    map.insert("type", m_type);
    map.insert("interval", m_interval);
    return map;
}

QString TimingInformation::type() const
{
    return m_type;
}

int TimingInformation::interval() const
{
    return m_interval;
}


GetConfigResponse::GetConfigResponse(QObject *parent)
: Response(parent)
, m_fetchTaskSchedule(new TimingInformation(this))
, m_keepaliveSchedule(new TimingInformation(this))
, m_updateConfigSchedule(new TimingInformation(this))
{
}

bool GetConfigResponse::fillFromVariant(const QVariantMap &variant)
{
    m_controllerAddress = variant.value("controller_address").toString();
    m_fetchTaskSchedule->fillFromVariant( qvariant_cast<QVariantMap>(variant.value("fetch_task_schedule")) );
    m_keepaliveAddress = variant.value("keepalive_address").toString();
    m_keepaliveSchedule->fillFromVariant( qvariant_cast<QVariantMap>(variant.value("keepalive_schedule")) );
    m_updateConfigSchedule->fillFromVariant( qvariant_cast<QVariantMap>(variant.value("update_config_schedule")) );

    emit responseChanged();
    return true;
}

QVariant GetConfigResponse::toVariant() const
{
    QVariantMap map;
    map.insert("controller_address", m_controllerAddress);
    map.insert("fetch_task_schedule", m_fetchTaskSchedule->toVariant());
    map.insert("keepalive_address", m_keepaliveAddress);
    map.insert("keepalive_schedule", m_keepaliveSchedule->toVariant());
    map.insert("update_config_schedule", m_updateConfigSchedule->toVariant());
    return map;
}

void GetConfigResponse::finished()
{
}

void GetConfigResponse::setControllerAddress(const QString &controllerAddress)
{
    m_controllerAddress = controllerAddress;
}

QString GetConfigResponse::controllerAddress() const
{
    return m_controllerAddress;
}

TimingInformation *GetConfigResponse::fetchTaskSchedule() const
{
    return m_fetchTaskSchedule;
}

QString GetConfigResponse::keepaliveAddress() const
{
    return m_keepaliveAddress;
}

TimingInformation *GetConfigResponse::keepaliveSchedule() const
{
    return m_keepaliveSchedule;
}

TimingInformation *GetConfigResponse::updateConfigSchedule() const
{
    return m_updateConfigSchedule;
}
