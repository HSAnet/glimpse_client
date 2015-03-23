#include "upnp_definition.h"

UPnPDefinition::UPnPDefinition(const bool mediaServerSearch)
    :mediaServerSearch(mediaServerSearch)
{

}

UPnPDefinition::~UPnPDefinition()
{
}

QVariant UPnPDefinition::toVariant() const
{
    QVariantMap map;
    map.insert("MediaServerSearch", mediaServerSearch);
    return map;
}

UPnPDefinitionPtr UPnPDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();
    return UPnPDefinitionPtr(new UPnPDefinition(map.value("MediaServerSearch").toBool()));
}
