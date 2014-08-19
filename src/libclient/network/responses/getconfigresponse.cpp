#include "getconfigresponse.h"
#include "client.h"
#include "settings.h"

GetConfigResponse::GetConfigResponse(QObject *parent)
: Response(parent)
{
}

bool GetConfigResponse::fillFromVariant(const QVariantMap &variant)
{
    m_supervisorChannel = Channel::fromVariant(variant.value("supervisor_channel"));
    m_keepaliveChannel = Channel::fromVariant(variant.value("keepalive_channel"));
    m_configChannel = Channel::fromVariant(variant.value("config_channel"));
    m_reportChannel = Channel::fromVariant(variant.value("report_channel"));

    return true;
}

QVariant GetConfigResponse::toVariant() const
{
    QVariantMap map;
    map.insert("supervisor_channel", m_supervisorChannel.toVariant());
    map.insert("keepalive_channel", m_keepaliveChannel.toVariant());
    map.insert("config_channel", m_configChannel.toVariant());
    map.insert("report_channel", m_reportChannel.toVariant());
    return map;
}

void GetConfigResponse::finished()
{
    // Sync the settings
    Client *client = Client::instance();
    Settings *settings = client->settings();
    settings->sync();

    emit responseChanged();
}

QString GetConfigResponse::supervisorAddress() const
{
    return m_supervisorChannel.target();
}

TimingPtr GetConfigResponse::supervisorTiming() const
{
    return m_supervisorChannel.timing();
}

QString GetConfigResponse::keepaliveAddress() const
{
    return m_keepaliveChannel.target();
}

TimingPtr GetConfigResponse::keepaliveTiming() const
{
    return m_keepaliveChannel.timing();
}

void GetConfigResponse::setConfigAddress(const QString &address)
{
    m_configChannel.setTarget(address);
}

QString GetConfigResponse::configAddress() const
{
    return m_configChannel.target();
}

TimingPtr GetConfigResponse::configTiming() const
{
    return m_configChannel.timing();
}

QString GetConfigResponse::reportAddress() const
{
    return m_reportChannel.target();
}

TimingPtr GetConfigResponse::reportTiming() const
{
    return m_reportChannel.timing();
}
