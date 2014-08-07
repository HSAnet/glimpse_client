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

DnslookupDefinitionPtr DnslookupDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return DnslookupDefinitionPtr(new DnslookupDefinition(map.value("host").toString(), map.value("dns_server").toString()));
}
