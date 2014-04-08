#ifndef MEASUREMENTDEFINITION_H
#define MEASUREMENTDEFINITION_H

#include "../serializable.h"

#include <QSharedPointer>
#include <QUuid>

class MeasurementDefinition;
typedef QSharedPointer<MeasurementDefinition> MeasurementDefinitionPtr;
typedef QList<MeasurementDefinitionPtr> MeasurementDefinitionPtrList;

class CLIENT_API MeasurementDefinition : public Serializable
{
public:
    MeasurementDefinition();
    ~MeasurementDefinition();

    QUuid measurementUuid;
};

#endif // MEASUREMENTDEFINITION_H
