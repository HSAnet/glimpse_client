#ifndef WIFI_LOOKUP_DEFINITION_H
#define WIFI_LOOKUP_DEFINITION_H

#include "../measurementdefinition.h"

class WifiLookupDefinition;

typedef QSharedPointer<WifiLookupDefinition> WifiLookupDefinitionPtr;
typedef QList<WifiLookupDefinitionPtr> WifiLookupDefinitionList;

class WifiLookupDefinition: public MeasurementDefinition
{
public:
    ~WifiLookupDefinition();
    WifiLookupDefinition();

    // Storage
    static WifiLookupDefinitionPtr fromVariant(const QVariant &variant);
    // Serializable interface
    QVariant toVariant() const;
};
#endif // WIFI_LOOKUP_DEFINITION_H
