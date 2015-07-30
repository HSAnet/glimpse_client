#include "snmp_definition.h"

// Constructs a SnmpDefinition object for all SNMP measurements
// except of a SingleRequest.
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

// Constructs a SnmpDefinition object for a SingleRequest.
// Is used if a user wants to request an agent. (Use Glimpse as a tiny Management System)
SnmpDefinition::SnmpDefinition(const int version, const QStringList &communityList, const QString &host, const QString &objectIdentifier,
                               const int authentication, const int privacy, const QString &username, const QString &password,
                               const QString &contextOID, const int measurementType) :
    m_snmpVersion(version),
    m_communityList(communityList),
    m_startRangeIp(host),
    m_objectIdentifier(objectIdentifier),
    m_authentication(authentication),
    m_privacy(privacy),
    m_username(username),
    m_password(password),
    m_contextOID(contextOID),
    m_measurementType(measurementType)
{

}

SnmpDefinition::~SnmpDefinition()
{

}

SnmpDefinitionPtr SnmpDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    if (map.value(QString("measurementType")).toInt() == 2)     // User send a request to an agent.
    {
        return SnmpDefinitionPtr(new SnmpDefinition(map.value(QString("snmpVersion")).toInt(),
                                                    map.value(QString("communityList")).toStringList(),
                                                    map.value(QString("startRangeIp")).toString(),
                                                    map.value(QString("objectIdentifier")).toString(),
                                                    map.value(QString("authentication")).toInt(),
                                                    map.value(QString("privacy")).toInt(),
                                                    map.value(QString("username")).toString(),
                                                    map.value(QString("password")).toString(),
                                                    map.value(QString("contextOID")).toString(),
                                                    map.value(QString("measurementType")).toInt()
                                                    ));
    }

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
    map.insert(QString("objectIdentifier"), m_objectIdentifier);
    map.insert(QString("authentication"), m_authentication);
    map.insert(QString("privacy"), m_privacy);
    map.insert(QString("username"), m_username);
    map.insert(QString("password"), m_password);
    map.insert(QString("contextOID"), m_contextOID);

    return QVariant(map);
}

