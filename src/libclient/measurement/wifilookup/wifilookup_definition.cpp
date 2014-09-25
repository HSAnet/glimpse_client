#include "wifilookup_definition.h"

WifiLookupDefinition::WifiLookupDefinition()
{
}

WifiLookupDefinition::~WifiLookupDefinition()
{
}

WifiLookupDefinitionPtr WifiLookupDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return WifiLookupDefinitionPtr();
}

QVariant WifiLookupDefinition::toVariant() const
{
    QVariantMap map;
    return map;
}
