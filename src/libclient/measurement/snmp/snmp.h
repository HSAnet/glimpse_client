#ifndef SNMP_MEASUREMENT_H
#define SNMP_MEASUREMENT_H

#include "measurement/measurement.h"
#include "snmp_definition.h"
#include "snmppaket.h"
#include <QNetworkInterface>

class Snmp : public Measurement
{
    Q_OBJECT
public:
    explicit Snmp(QObject *parent = 0);
    ~Snmp();

signals:

public slots:

    // Measurement interface
public:
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

private:
    Status m_status;
};

#endif // SNMP_MEASUREMENT_H
