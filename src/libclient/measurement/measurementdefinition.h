#ifndef MEASUREMENTDEFINITION_H
#define MEASUREMENTDEFINITION_H

#include "../serializable.h"

#include <QSharedPointer>

class MeasurementDefinition;
typedef QSharedPointer<MeasurementDefinition> MeasurementDefinitionPtr;
typedef QList<MeasurementDefinitionPtr> MeasurementDefinitionPtrList;

class CLIENT_API MeasurementDefinition : public Serializable
{
public:
    MeasurementDefinition();
    ~MeasurementDefinition();
};

#endif // MEASUREMENTDEFINITION_H
