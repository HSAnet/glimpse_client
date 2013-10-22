#ifndef PING_H
#define PING_H

#include "../measurement.h"
#include "ping_definition.h"

#include <QProcess>
#include <QTextStream>

class Ping : public Measurement
{
    Q_OBJECT

public:
    explicit Ping(QObject *parent = 0);
    ~Ping();

    // Measurement interface
    Status status() const;
    bool prepare(NetworkManager* networkManager, const MeasurementDefinitionPtr& measurementDefinition);
    bool start();
    bool stop();
    void waitForFinished();
    int averagePingTime();
    ResultPtr result() const;

private:
    void setStatus(Status status);

    PingDefinitionPtr definition;
    QList<int> pingTime;
    QProcess process;
    QTextStream stream;
    Status currentStatus;

private slots:
    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyRead();

signals:
    void statusChanged(Status status);
    void ping(int time);
};

#endif // PING_H
