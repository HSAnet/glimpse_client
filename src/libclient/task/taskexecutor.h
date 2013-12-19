#ifndef TASKEXECUTOR_H
#define TASKEXECUTOR_H

#include "task.h"
#include "../report/report.h"

#include <QObject>

class NetworkManager;

#include "../measurement/measurement.h"
class MeasurementObserver
{
public:
    virtual ~MeasurementObserver() {}
    virtual void created(const MeasurementPtr& measurement) = 0;
};

class TaskExecutor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
    TaskExecutor();
    ~TaskExecutor();

    void setNetworkManager(NetworkManager* networkManager);
    NetworkManager* networkManager() const;

    bool isRunning() const;

    void execute(const TestDefinitionPtr &test, MeasurementObserver* observer = NULL);

signals:
    void runningChanged(bool running);

    void started(const TestDefinitionPtr& test);
    void finished(const TestDefinitionPtr& test, const ResultPtr& result);

protected:
    class Private;
    Private* d;
};


#endif // TASKEXECUTOR_H
