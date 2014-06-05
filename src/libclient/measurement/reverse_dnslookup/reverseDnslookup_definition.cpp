#include "reverseDnslookup_definition.h"

ReverseDnslookupDefinition::ReverseDnslookupDefinition(const QString &ip)
: ip(ip)
{
}

ReverseDnslookupDefinition::~ReverseDnslookupDefinition()
{
}

QVariant ReverseDnslookupDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("ip", ip);
    return map;
}

ReverseDnslookupDefinitionPtr ReverseDnslookupDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return ReverseDnslookupDefinitionPtr(new ReverseDnslookupDefinition(map.value("ip").toString()));
}
