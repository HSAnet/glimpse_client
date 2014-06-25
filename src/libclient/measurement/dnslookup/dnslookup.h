#ifndef DNSLOOKUP_H
#define DNSLOOKUP_H

#include "../measurement.h"
#include "dnslookup_definition.h"

#include <QDnsLookup>

class Dnslookup : public Measurement
{
    Q_OBJECT

public:
    explicit Dnslookup(QObject *parent = 0);
    ~Dnslookup();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

private:
    void setStatus(Status status);

    DnslookupDefinitionPtr m_definition;
    QDnsLookup m_dns;
    QList<QDnsHostAddressRecord> m_dnslookupOutput;
    QString m_dnsError;
    Status m_currentStatus;

private slots:
    void started();
    void finished();
    void handleServers();

signals:
    void statusChanged(Status status);

};

#endif // DNSLOOKUP_H
