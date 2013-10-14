#ifndef BTC_DEFINITION_H
#define BTC_DEFINITION_H

#include "../measurementdefinition.h"

class BulkTransportCapacityDefinition;

typedef QSharedPointer<BulkTransportCapacityDefinition> BulkTransportCapacityDefinitionPtr;
typedef QList<BulkTransportCapacityDefinitionPtr> BulkTransportCapacityDefinitionList;

class BulkTransportCapacityDefinition : public MeasurementDefinition
{
public:
    BulkTransportCapacityDefinition(QString host, quint16 port, quint64 initialDataSize);

    QString host;
    quint16 port;
    quint64 initialDataSize;

    QVariant toVariant() const;
    static BulkTransportCapacityDefinitionPtr fromVariant(const QVariant &variant);
};

#endif // BTC_DEFINITION_H
