#ifndef UDPPING_DEFINITION_H
#define UDPPING_DEFINITION_H

#include "../measurementdefinition.h"
#include "../../types.h"
#include <QAbstractSocket>

class PingDefinition;

typedef QSharedPointer<PingDefinition> PingDefinitionPtr;
typedef QList<PingDefinitionPtr> PingDefinitionList;

class PingDefinition : public MeasurementDefinition
{
public:
    ~PingDefinition();
    PingDefinition(const QString &host, const quint32 &count, const quint32 &interval, const quint32 &receiveTimeout,
                      const int &ttl, const quint16 &destinationPort, const quint16 &sourcePort, const quint32 &payload,
                      const ping::PingType &type);

    // Storage
    static PingDefinitionPtr fromVariant(const QVariant &variant);
    static PingDefinitionPtr fromSpecification(const QVariant &variant);

    // Getters
    QString host;
    quint32 count;
    quint32 interval;
    quint32 receiveTimeout;
    int ttl;
    quint16 destinationPort;
    quint16 sourcePort;
    quint32 payload;
    ping::PingType type;

    // Serializable interface
    QVariant toVariant() const;

    static QVariantMap capability();
};

#endif // UDPPING_DEFINITION_H
