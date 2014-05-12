#ifndef REVERSEDNSLOOKUP_H
#define REVERSEDNSLOOKUP_H

#include "../measurement.h"
#include "reverse_dnslookup_definition.h"

#include <QHostInfo>

class ReverseDnslookup : public Measurement
{
    Q_OBJECT

public:
    explicit ReverseDnslookup(QObject *parent = 0);
    ~ReverseDnslookup();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition);
    bool start();
    bool stop();
    ResultPtr result() const;

private:
    void setStatus(Status status);

    ReverseDnslookupDefinitionPtr definition;
    QHostInfo lookupHost;
    QList<QString> reverse_dnslookupOutput;
    Status currentStatus;

private slots:
    void started();
    void finished();
    void handleServers();

signals:
    void statusChanged(Status status);

};

#endif // REVERSEDNSLOOKUP_H
