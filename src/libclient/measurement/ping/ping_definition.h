#ifndef PING_DEFINITION_H
#define PING_DEFINITION_H

#include "../measurementdefinition.h"

class PingDefinition;

typedef QSharedPointer<PingDefinition> PingDefinitionPtr;
typedef QList<PingDefinitionPtr> PingDefinitionList;

class PingDefinition : public MeasurementDefinition
{
public:
    PingDefinition(const QString& host, quint16 count, quint16 timeout, quint16 interval);
    ~PingDefinition();

    // Storage
    static PingDefinitionPtr fromVariant(const QVariant &variant);

    // Getters
    QString host;
    quint16 count;
    quint32 timeout; // in ms
    quint32 interval; // in ms

    // Serializable interface
    QVariant toVariant() const;
};

#endif // PING_DEFINITION_H
