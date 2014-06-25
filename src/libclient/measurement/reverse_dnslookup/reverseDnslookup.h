#ifndef REVERSEDNSLOOKUP_H
#define REVERSEDNSLOOKUP_H

#include "../measurement.h"
#include "reverseDnslookup_definition.h"

#include <QHostInfo>

class ReverseDnslookup : public Measurement
{
    Q_OBJECT

public:
    explicit ReverseDnslookup(QObject *parent = 0);
    ~ReverseDnslookup();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition);
    bool start();
    bool stop();
    Result result() const;

private:
    void setStatus(Status status);

    Status m_currentStatus;
    int m_lookupId;
    ReverseDnslookupDefinitionPtr m_definition;
    QString m_reverseDnslookupOutput;
    QList<QHostAddress> m_reverseDnslookupAddresses;
    QString m_reverseDnslookupError;

private slots:
    void started();
    void finished();
    void handleServers(QHostInfo info);

signals:
    void statusChanged(Status status);

};

#endif // REVERSEDNSLOOKUP_H
