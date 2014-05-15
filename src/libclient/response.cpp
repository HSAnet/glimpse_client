#include "response.h"
#include "client.h"
#include "settings.h"

#include "timing/timingfactory.h"
#include "channel.h"

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
    m_apiKey = variant.value("api_key").toString();
    m_registeredDevice = variant.value("registered_device").toBool();

    return !m_apiKey.isEmpty();
}

void LoginResponse::finished()
{
    Client* client = Client::instance();

    // Update the settings
    Settings* settings = client->settings();
    settings->setApiKey(m_apiKey);

    // Set registered status (after settings!)
    client->setStatus(Client::Registered);
}

QString LoginResponse::apiKey() const
{
    return m_apiKey;
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
    m_supervisor_channel = ChannelPtr(new Channel());
    m_keepalive_channel = ChannelPtr(new Channel());
    m_config_channel = ChannelPtr(new Channel());
    m_report_channel = ChannelPtr(new Channel());
}

bool GetConfigResponse::fillFromVariant(const QVariantMap &variant)
{
    m_supervisor_channel = Channel::fromVariant(variant.value("supervisor_channel"));
    m_keepalive_channel = Channel::fromVariant(variant.value("keepalive_channel"));
    m_config_channel = Channel::fromVariant(variant.value("config_channel"));
    m_report_channel = Channel::fromVariant(variant.value("report_channel"));

    return true;
}

QVariant GetConfigResponse::toVariant() const
{
    QVariantMap map;
    map.insert("supervisor_channel", m_supervisor_channel->toVariant());
    map.insert("keepalive_channel", m_keepalive_channel->toVariant());
    map.insert("config_channel", m_config_channel->toVariant());
    map.insert("report_channel", m_report_channel->toVariant());
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

QString GetConfigResponse::supervisorAddress() const
{
    return m_supervisor_channel->target();
}

TimingPtr GetConfigResponse::supervisorTiming() const
{
    return m_supervisor_channel->timing();
}

QString GetConfigResponse::keepaliveAddress() const
{
    return m_keepalive_channel->target();
}

TimingPtr GetConfigResponse::keepaliveTiming() const
{
    return m_keepalive_channel->timing();
}

void GetConfigResponse::setConfigAddress(const QString address)
{
    m_config_channel->setTarget(address);
}

QString GetConfigResponse::configAddress() const
{
    return m_config_channel->target();
}

TimingPtr GetConfigResponse::configTiming() const
{
    return m_config_channel->timing();
}

QString GetConfigResponse::reportAddress() const
{
    return m_report_channel->target();
}

TimingPtr GetConfigResponse::reportTiming() const
{
    return m_report_channel->timing();
}
