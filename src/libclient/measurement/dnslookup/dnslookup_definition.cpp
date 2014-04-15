#include "dnslookup_definition.h"

DnslookupDefinition::DnslookupDefinition(const QString& host)
: host(host)
{
}

DnslookupDefinition::~DnslookupDefinition()
{
}

QVariant DnslookupDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("host", host);
    return map;
}

DnslookupDefinitionPtr DnslookupDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return DnslookupDefinitionPtr(new DnslookupDefinition(map.value("host").toString()));
}
