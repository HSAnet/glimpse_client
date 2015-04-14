#ifndef SNMPDEFINITION_H
#define SNMPDEFINITION_H

#include "measurement/measurement.h"


class SnmpDefinition;

typedef QSharedPointer<SnmpDefinition> SnmpDefinitionPtr;
typedef QList<SnmpDefinitionPtr> DnslookupDefinitionList;

class SnmpDefinition : public MeasurementDefinition
{
public:
    SnmpDefinition();
    ~SnmpDefinition();

    static SnmpDefinitionPtr fromVariant(const QVariant &variant);
    QVariant toVariant() const;
};

#endif // SNMPDEFINITION_H
