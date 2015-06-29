#ifndef REVERSEDNSLOOKUP_DEFINITION_H
#define REVERSEDNSLOOKUP_DEFINITION_H

#include "../measurementdefinition.h"

class ReverseDnslookupDefinition;

typedef QSharedPointer<ReverseDnslookupDefinition> ReverseDnslookupDefinitionPtr;
typedef QList<ReverseDnslookupDefinitionPtr> ReverseDnslookupDefinitionList;

class ReverseDnslookupDefinition : public MeasurementDefinition
{
public:
    ReverseDnslookupDefinition(const QString &ip);
    ~ReverseDnslookupDefinition();

    // Storage
    static ReverseDnslookupDefinitionPtr fromVariant(const QVariant &variant);
    static ReverseDnslookupDefinitionPtr fromSpecification(const QVariant &variant);

    // Getters
    QString ip;

    // Serializable interface
    QVariant toVariant() const;

    static QVariantMap capability();
};

#endif // REVERSEDNSLOOKUP_DEFINITION_H
