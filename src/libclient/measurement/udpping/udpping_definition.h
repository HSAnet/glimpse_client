#ifndef UDPPING_DEFINITION_H
#define UDPPING_DEFINITION_H

#include "../measurementdefinition.h"

class UdpPingDefinition;

typedef QSharedPointer<UdpPingDefinition> UdpPingDefinitionPtr;
typedef QList<UdpPingDefinitionPtr> UdpPingDefinitionList;

class UdpPingDefinition : public MeasurementDefinition
{
public:
    ~UdpPingDefinition();
    UdpPingDefinition(const QString &url, const quint32 &count, const quint32 &interval, const quint32 &receiveTimeout,
                      const int &ttl, const quint16 &destinationPort, const quint16 &sourcePort, const QByteArray &payload);

    // Storage
    static UdpPingDefinitionPtr fromVariant(const QVariant &variant);

    // Getters
    QString url;
    //QHostAddress m_hostAddress;
    quint32 count;
    quint32 interval;
    quint32 receiveTimeout;
    int ttl;
    quint16 destinationPort;
    quint16 sourcePort;
    QByteArray payload;

    // Serializable interface
    QVariant toVariant() const;
};

#endif // UDPPING_DEFINITION_H
