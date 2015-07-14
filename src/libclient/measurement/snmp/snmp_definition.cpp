#include "snmp_definition.h"

SnmpDefinition::SnmpDefinition(const QStringList &communityList, int retriesPerIp, const int snmpVersion, const QString &rangeStartIp,
                               const QString &rangeEndIp, const int measurementType, const int sendInterval, const int waitTime) :
    m_communityList(communityList),
    m_retriesPerIp(retriesPerIp),
    m_snmpVersion(snmpVersion),
    m_startRangeIp(rangeStartIp),
    m_endRangeIp(rangeEndIp),
    m_measurementType(measurementType),
    m_sendInterval(sendInterval),
    m_waitTime(waitTime)
{
}

SnmpDefinition::~SnmpDefinition()
{

}

SnmpDefinitionPtr SnmpDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return SnmpDefinitionPtr(new SnmpDefinition(map.value(QString("communityList")).toStringList(),
                                                map.value(QString("retriesPerIp")).toInt(),
                                                map.value(QString("snmpVersion")).toInt(),
                                                map.value(QString("startRangeIp")).toString(),
                                                map.value(QString("endRangeIp")).toString(),
                                                map.value(QString("measurementType")).toInt(),
                                                map.value(QString("sendInterval")).toInt(),
                                                map.value(QString("waitTime")).toInt()));
}

QVariant SnmpDefinition::toVariant() const
{
    QVariantMap map;
    map.insert(QString("communityList"), m_communityList);
    map.insert(QString("retriesPerIp"), m_retriesPerIp);
    map.insert(QString("snmpVersion"), m_snmpVersion);
    map.insert(QString("startRangeIp"), m_startRangeIp.toString());
    map.insert(QString("endRangeIp"), m_endRangeIp.toString());
    map.insert(QString("measurementType"), m_measurementType);
    map.insert(QString("sendInterval"), m_sendInterval);
    map.insert(QString("waitTime"), m_waitTime);

    return QVariant(map);
}

