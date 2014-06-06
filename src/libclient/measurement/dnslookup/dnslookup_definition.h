#ifndef Dnslookup_DEFINITION_H
#define Dnslookup_DEFINITION_H

#include "../measurementdefinition.h"

class DnslookupDefinition;

typedef QSharedPointer<DnslookupDefinition> DnslookupDefinitionPtr;
typedef QList<DnslookupDefinitionPtr> DnslookupDefinitionList;

class DnslookupDefinition : public MeasurementDefinition
{
public:
    DnslookupDefinition(const QString &host, const QString &dnsServer = QString());
    ~DnslookupDefinition();

    // Storage
    static DnslookupDefinitionPtr fromVariant(const QVariant &variant);

    // Getters
    QString host;
    QString dnsServer;

    // Serializable interface
    QVariant toVariant() const;
};

#endif // Dnslookup_DEFINITION_H
