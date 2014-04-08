#ifndef Dnslookup_H
#define Dnslookup_H

#include "../measurement.h"
#include "dnslookup_definition.h"

#include <QProcess>
#include <QTextStream>

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
    void waitForFinished();
    float averageDnslookupTime();
    ResultPtr result() const;

private:
    void setStatus(Status status);

    DnslookupDefinitionPtr definition;
    QList<float> dnslookupTime;
    QProcess process;
    QTextStream stream;
    Status currentStatus;

private slots:
    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyRead();

signals:
    void statusChanged(Status status);
    void dnslookup(int time);
};

#endif // Dnslookup_H
