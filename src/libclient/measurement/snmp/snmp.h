#ifndef SNMP_MEASUREMENT_H
#define SNMP_MEASUREMENT_H

#include "measurement/measurement.h"
#include "snmp_definition.h"
#include "snmppacket.h"
#include "snmpscanner.h"
#include "resultcreator.h"
#include <QNetworkInterface>

class Snmp : public Measurement
{
    Q_OBJECT
public:
    explicit Snmp(QObject *parent = 0);
    ~Snmp();

signals:

public slots:
    void finaliseMeasurement();
    void errorReport(const QString errorMsg);

    // Measurement interface
public:
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

    enum Type { AutoScan, RangeScan, SingleRequest, GatewayRequest };

private:
    Status m_status;
    SnmpDefinitionPtr m_definition;
    SnmpScanner m_scanner;
    ResultCreator m_resultCreator;
};

#endif // SNMP_MEASUREMENT_H
