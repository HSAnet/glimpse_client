#include "response.h"
#include "client.h"
#include "settings.h"

#include "timing/timingfactory.h"

Response::Response(QObject *parent)
: QObject(parent)
{
}

Response::~Response()
{
}

void Response::finished()
{
}

LoginResponse::LoginResponse(QObject *parent)
: Response(parent)
, m_registeredDevice(false)
{
}

bool LoginResponse::fillFromVariant(const QVariantMap &variant)
{
    m_sessionId = variant.value("session_id").toString();
    m_registeredDevice = variant.value("registered_device", true).toBool();

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


GetConfigResponse::GetConfigResponse(QObject *parent)
: Response(parent)
{
}

bool GetConfigResponse::fillFromVariant(const QVariantMap &variant)
{
    m_controllerAddress = variant.value("controller_address").toString();
    m_fetchTaskSchedule = TimingFactory::timingFromVariant(variant.value("fetch_task_schedule"));
    m_keepaliveAddress = variant.value("keepalive_address").toString();
    m_keepaliveSchedule = TimingFactory::timingFromVariant(variant.value("keepalive_schedule"));
    m_updateConfigSchedule = TimingFactory::timingFromVariant(variant.value("update_config_schedule"));
    m_reportSchedule = TimingFactory::timingFromVariant(variant.value("report_schedule"));

    return true;
}

QVariant GetConfigResponse::toVariant() const
{
    if (m_fetchTaskSchedule.isNull())
    {
        return QVariant();
    }

    QVariantMap map;
    map.insert("controller_address", m_controllerAddress);
    map.insert("fetch_task_schedule", m_fetchTaskSchedule->toVariant());
    map.insert("keepalive_address", m_keepaliveAddress);
    map.insert("keepalive_schedule", m_keepaliveSchedule->toVariant());
    map.insert("update_config_schedule", m_updateConfigSchedule->toVariant());
    map.insert("report_schedule", m_reportSchedule->toVariant());
    return map;
}

void GetConfigResponse::finished()
{
    // Sync the settings
    Client* client = Client::instance();
    Settings* settings = client->settings();
    settings->sync();

    emit responseChanged();
}

void GetConfigResponse::setControllerAddress(const QString &controllerAddress)
{
    m_controllerAddress = controllerAddress;
}

QString GetConfigResponse::controllerAddress() const
{
    return m_controllerAddress;
}

TimingPtr GetConfigResponse::fetchTaskSchedule() const
{
    return m_fetchTaskSchedule;
}

QString GetConfigResponse::keepaliveAddress() const
{
    return m_keepaliveAddress;
}

TimingPtr GetConfigResponse::keepaliveSchedule() const
{
    return m_keepaliveSchedule;
}

TimingPtr GetConfigResponse::updateConfigSchedule() const
{
    return m_updateConfigSchedule;
}

TimingPtr GetConfigResponse::reportSchedule() const
{
    return m_reportSchedule;
}
