#ifndef SNMPDEFINITION_H
#define SNMPDEFINITION_H

#include "measurement/measurement.h"


class SnmpDefinition;

typedef QSharedPointer<SnmpDefinition> SnmpDefinitionPtr;
typedef QList<SnmpDefinitionPtr> SnmpDefinitionList;

class SnmpDefinition : public MeasurementDefinition
{
public:
    SnmpDefinition(const QStringList &communityList, const int retriesPerIp, const int snmpVersion, const int startRangeIp,
                   const int endRangeIp, const int measurementType, const int sendInterval, const int waitTime);
    ~SnmpDefinition();

    QStringList m_communityList;
    int m_retriesPerIp;
    int m_snmpVersion;
    int m_startRangeIp;
    int m_endRangeIp;
    int m_measurementType;
    int m_sendInterval;
    int m_waitTime;

    static SnmpDefinitionPtr fromVariant(const QVariant &variant);
    QVariant toVariant() const;
};

#endif // SNMPDEFINITION_H
