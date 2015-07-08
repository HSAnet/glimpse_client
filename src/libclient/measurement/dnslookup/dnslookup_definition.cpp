#include "dnslookup_definition.h"

DnslookupDefinition::DnslookupDefinition(const QString &host, const QString &dnsServer)
: host(host)
, dnsServer(dnsServer)
{
}

DnslookupDefinition::~DnslookupDefinition()
{
}

QVariant DnslookupDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("host", host);
    map.insert("dns_server", dnsServer);
    return map;
}

QVariantMap DnslookupDefinition::capability()
{
    QVariantMap parameters;
    parameters.insert("destination.url", "*");
    parameters.insert("glimpse.dns.server", "*");

    QVariantList results;
    results.append("glimpse.dns.record-name");
    results.append("ttl");
    results.append("glimpse.dns.record-value");
    results.append("glimpse.dns.record-type");


    QVariantMap capability;
    capability.insert("capability", "measure");
    capability.insert("label", "glimpse-dnslookup");
    capability.insert("parameters", parameters);
    capability.insert("results", results);
    capability.insert("version", 0);
    capability.insert("when", "now");
    capability.insert("registry", "https://www.measure-it.net/static/glimpse_registry.json");

    return capability;
}

DnslookupDefinitionPtr DnslookupDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    if (map.isEmpty())
    {
        return DnslookupDefinitionPtr();
    }

    return DnslookupDefinitionPtr(new DnslookupDefinition(map.value("host").toString(), map.value("dns_server").toString()));
}

DnslookupDefinitionPtr DnslookupDefinition::fromSpecification(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    if (map.isEmpty())
    {
        return DnslookupDefinitionPtr();
    }

    return DnslookupDefinitionPtr(new DnslookupDefinition(map.value("destination.url").toString(), map.value("glimpse.dns.server").toString()));
}
