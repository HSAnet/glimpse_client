#ifndef UPNP_DEFINITION_H
#define UPNP_DEFINITION_H

#include "../measurementdefinition.h"

class UPnPDefinition;

typedef QSharedPointer<UPnPDefinition> UPnPDefinitionPtr;
typedef QList<UPnPDefinitionPtr> UpnpDefinitionList;

class UPnPDefinition : public MeasurementDefinition
{
public:
    UPnPDefinition(const bool mediaServerSearch);
    ~UPnPDefinition();

    // Storage
    static UPnPDefinitionPtr fromVariant(const QVariant &variant);

    // Getters
    bool mediaServerSearch;

    // Serializable interface
    QVariant toVariant() const;
};

#endif // UPNP_DEFINITION_H
