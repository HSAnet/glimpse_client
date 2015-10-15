#ifndef UPNPDEFINITION_H
#define UPNPDEFINITION_H

#include "../measurementdefinition.h"
#include "../../types.h"

class UpnpDefinition;

typedef QSharedPointer<UpnpDefinition> UpnpDefinitionPtr;
typedef QList<UpnpDefinitionPtr> UpnpDefinitionList;

class CLIENT_API UpnpDefinition : public MeasurementDefinition
{
public:
    ~UpnpDefinition() {}
    UpnpDefinition() {}

    static UpnpDefinitionPtr fromVariant(const QVariant &variant)
    {
        Q_UNUSED(variant);
        return UpnpDefinitionPtr();
    }

    // Serializable interface
    QVariant toVariant() const
    {
        return QVariant();
    }
};

#endif // UPNPDEFINITION_H
