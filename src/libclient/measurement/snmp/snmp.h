#ifndef SNMP_MEASUREMENT_H
#define SNMP_MEASUREMENT_H

#include "../measurement.h"
#include "snmp_definition.h"
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
    void snmpMeasurementFinished();
    void snmpErrorReport(const QString errorMsg);

    // Measurement interface
public:
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

    enum Type { AutoScan, RangeScan, SingleRequest, GatewayStatistics };
    enum RequestType { GetRequest, SetRequest };

private:
    Status m_status;
    SnmpDefinitionPtr m_definition;
    SnmpScanner m_scanner;
    ResultCreator m_resultCreator;

    // Private Methods
    void setStatus(Measurement::Status status);
    QString getDefaultGateway();
    bool getGatewaySatistics();
    bool userSingleRequest();
    QHostAddress dnsLookup(const QString &hostname) const;
};

#endif // SNMP_MEASUREMENT_H
