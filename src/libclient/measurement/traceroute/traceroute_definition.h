#ifndef TRACEROUTE_DEFINITION_H
#define TRACEROUTE_DEFINITION_H

#include "../measurementdefinition.h"
#include <QAbstractSocket>

class TracerouteDefinition;

typedef QSharedPointer<TracerouteDefinition> TracerouteDefinitionPtr;
typedef QList<TracerouteDefinitionPtr> TracerouteDefinitionList;

class TracerouteDefinition : public MeasurementDefinition
{
public:
    TracerouteDefinition(const QString &url, const quint32 &count,
                         const quint32 &interval, const quint32 &receiveTimeout,
                         const quint16 &destinationPort,
                         const quint16 &sourcePort, const quint32 &payload,
                         const QAbstractSocket::SocketType &pingType);
    ~TracerouteDefinition();

    // Storage
    static TracerouteDefinitionPtr fromVariant(const QVariant &variant);

    // Getter
    QString url;
    quint32 count;
    quint32 interval;
    quint32 receiveTimeout;
    quint16 destinationPort;
    quint16 sourcePort;
    quint32 payload;
    QAbstractSocket::SocketType pingType;

    // Serializable interface
    QVariant toVariant() const;
};

#endif // TRACEROUTE_DEFINITION_H
