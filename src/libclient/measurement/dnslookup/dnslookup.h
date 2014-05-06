#ifndef Dnslookup_H
#define Dnslookup_H

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
    bool prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    void setStatus(Status status);
    void handleServers();
    DnslookupDefinitionPtr definition;
    QDnsLookup dns;
    QList<QString> dnslookupOutput;
//    QTextStream stream;
    Status currentStatus;

private slots:
    void started();
    void finished();

signals:
    void statusChanged(Status status);
    void dnslookup(int time);
};

#endif // Dnslookup_H
