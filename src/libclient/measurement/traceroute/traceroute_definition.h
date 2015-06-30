#ifndef TRACEROUTE_DEFINITION_H
#define TRACEROUTE_DEFINITION_H

#include "../measurementdefinition.h"
#include "../../types.h"
#include <QAbstractSocket>

class TracerouteDefinition;

typedef QSharedPointer<TracerouteDefinition> TracerouteDefinitionPtr;
typedef QList<TracerouteDefinitionPtr> TracerouteDefinitionList;

class TracerouteDefinition : public MeasurementDefinition
{
public:
    TracerouteDefinition(const QString &host, const quint32 &count,
                         const quint32 &interval, const quint32 &receiveTimeout,
                         const quint16 &destinationPort,
                         const quint16 &sourcePort, const quint32 &payload,
                         const ping::PingType &type);
    ~TracerouteDefinition();

    // Storage
    static TracerouteDefinitionPtr fromVariant(const QVariant &variant);

    // Getter
    QString host;
    quint32 count;
    quint32 interval;
    quint32 receiveTimeout;
    quint16 destinationPort;
    quint16 sourcePort;
    quint32 payload;
    ping::PingType type;

    // Serializable interface
    QVariant toVariant() const;

    static QVariantMap capability();
};

#endif // TRACEROUTE_DEFINITION_H
