#ifndef BTC_DEFINITION_H
#define BTC_DEFINITION_H

#include "../measurementdefinition.h"

class BulkTransportCapacityDefinition;

typedef QSharedPointer<BulkTransportCapacityDefinition> BulkTransportCapacityDefinitionPtr;
typedef QList<BulkTransportCapacityDefinitionPtr> BulkTransportCapacityDefinitionList;

class BulkTransportCapacityDefinition : public MeasurementDefinition
{
public:
    BulkTransportCapacityDefinition(const QString& host, quint16 port, quint64 initialDataSize);
    ~BulkTransportCapacityDefinition();

    // Storage
    static BulkTransportCapacityDefinitionPtr fromVariant(const QVariant &variant);

    // Getters
    QString host;
    quint16 port;
    quint64 initialDataSize;

    // Serializable interface
    QVariant toVariant() const;
};

#endif // BTC_DEFINITION_H
