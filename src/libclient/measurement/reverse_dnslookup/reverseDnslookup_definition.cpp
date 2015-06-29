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
    map.insert("destination.ip4", ip);
    return map;
}

ReverseDnslookupDefinitionPtr ReverseDnslookupDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    if (map.isEmpty())
    {
        return ReverseDnslookupDefinitionPtr();
    }

    return ReverseDnslookupDefinitionPtr(new ReverseDnslookupDefinition(map.value("ip").toString()));
}

ReverseDnslookupDefinitionPtr ReverseDnslookupDefinition::fromSpecification(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    if (map.isEmpty())
    {
        return ReverseDnslookupDefinitionPtr();
    }

    return ReverseDnslookupDefinitionPtr(new ReverseDnslookupDefinition(map.value("destination.ip4").toString()));
}

QVariantMap ReverseDnslookupDefinition::capability()
{
    QVariantMap parameters;
    parameters.insert("destination.ip4", "*");

    QVariantList results;
    results.append("glimpse.dns.record-name");
    results.append("glimpse.dns.record-value");


    QVariantMap capability;
    capability.insert("capability", "measure");
    capability.insert("label", "glimpse-reversednslookup");
    capability.insert("parameters", parameters);
    capability.insert("results", results);
    capability.insert("version", 0);
    capability.insert("when", "now");
    capability.insert("registry", "https://www.measure-it.net/static/glimpse_registry.json");

    return capability;
}
