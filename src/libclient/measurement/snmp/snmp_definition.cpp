#include "snmp_definition.h"

// Constructs a SnmpDefinition object for all SNMP measurements
// except of a SingleRequest.
SnmpDefinition::SnmpDefinition(const QStringList &communityList, int retriesPerIp, const int snmpVersion, const QString &hostAddresses,
                               const int measurementType, const int sendInterval, const int waitTime) :
    m_communityList(communityList),
    m_retriesPerIp(retriesPerIp),
    m_snmpVersion(snmpVersion),
    m_hostAddresses(hostAddresses),
    m_measurementType(measurementType),
    m_sendInterval(sendInterval),
    m_waitTime(waitTime)
{
}

// Constructs a SnmpDefinition object for a SingleRequest.
// Is used if a user wants to request an agent. (Use Glimpse as a tiny Management System)
SnmpDefinition::SnmpDefinition(const int version, const int requestType,const QString &communityName, const QString &host,
                               const QString &objectIdentifier, const QString &value, const int authentication, const int privacy,
                               const QString &username, const QString &password, const QString &contextOID, const int measurementType) :
    m_snmpVersion(version),
    m_communityName(communityName),
    m_hostAddresses(host),
    m_objectIdentifier(objectIdentifier),
    m_authentication(authentication),
    m_privacy(privacy),
    m_username(username),
    m_password(password),
    m_contextOID(contextOID),
    m_measurementType(measurementType),
    m_requestType(requestType),
    m_requestValue(value)
{

}

SnmpDefinition::~SnmpDefinition()
{

}

SnmpDefinitionPtr SnmpDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    int measurementType = map.value(QString("measurementType")).toInt();

    if (measurementType == 2)     // User send a request to an agent (SingleRequest).
    {
        return SnmpDefinitionPtr(new SnmpDefinition(map.value(QString("snmpVersion")).toInt(),
                                                    map.value(QString("requestType")).toInt(),
                                                    map.value(QString("communityName")).toString(),
                                                    map.value(QString("hostAddresses")).toString(),
                                                    map.value(QString("objectIdentifier")).toString(),
                                                    map.value(QString("requestValue")).toString(),
                                                    map.value(QString("authentication")).toInt(),
                                                    map.value(QString("privacy")).toInt(),
                                                    map.value(QString("username")).toString(),
                                                    map.value(QString("password")).toString(),
                                                    map.value(QString("contextOID")).toString(),
                                                    map.value(QString("measurementType")).toInt()
                                                    ));
    }

    // Scan subnet or IP range
    return SnmpDefinitionPtr(new SnmpDefinition(map.value(QString("communityList")).toStringList(),
                                                map.value(QString("retriesPerIp")).toInt(),
                                                map.value(QString("snmpVersion")).toInt(),
                                                map.value(QString("hostAddresses")).toString(),
                                                map.value(QString("measurementType")).toInt(),
                                                map.value(QString("sendInterval")).toInt(),
                                                map.value(QString("waitTime")).toInt()));
}

QVariant SnmpDefinition::toVariant() const
{
    QVariantMap map;
    map.insert(QString("communityName"), m_communityName);
    map.insert(QString("communityList"), m_communityList);
    map.insert(QString("retriesPerIp"), m_retriesPerIp);
    map.insert(QString("snmpVersion"), m_snmpVersion);
    map.insert(QString("hostAddresses"), m_hostAddresses);
    map.insert(QString("measurementType"), m_measurementType);
    map.insert(QString("sendInterval"), m_sendInterval);
    map.insert(QString("waitTime"), m_waitTime);
    map.insert(QString("objectIdentifier"), m_objectIdentifier);
    map.insert(QString("authentication"), m_authentication);
    map.insert(QString("privacy"), m_privacy);
    map.insert(QString("username"), m_username);
    map.insert(QString("password"), m_password);
    map.insert(QString("contextOID"), m_contextOID);
    map.insert(QString("requestType"), m_requestType);
    map.insert(QString("requestValue"), m_requestValue);

    return QVariant(map);
}

