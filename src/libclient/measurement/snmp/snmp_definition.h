#ifndef SNMPDEFINITION_H
#define SNMPDEFINITION_H

#include "measurement/measurement.h"
#include <QHostAddress>


class SnmpDefinition;

typedef QSharedPointer<SnmpDefinition> SnmpDefinitionPtr;
typedef QList<SnmpDefinitionPtr> SnmpDefinitionList;

class SnmpDefinition : public MeasurementDefinition
{
public:
    SnmpDefinition(const QStringList &communityList, const int retriesPerIp, const int snmpVersion, const QString &rangeStartIp,
                   const QString &rangeEndIp, const int measurementType, const int sendInterval, const int waitTime);
    ~SnmpDefinition();

    QStringList m_communityList;
    int m_retriesPerIp;
    int m_snmpVersion;
    QHostAddress m_startRangeIp;
    QHostAddress m_endRangeIp;
    int m_measurementType;
    int m_sendInterval;
    int m_waitTime;

    static SnmpDefinitionPtr fromVariant(const QVariant &variant);
    QVariant toVariant() const;
};

#endif // SNMPDEFINITION_H
