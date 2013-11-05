#ifndef TASKEXECUTOR_H
#define TASKEXECUTOR_H

#include "task.h"
#include "../report/report.h"

#include <QObject>

class NetworkManager;

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

    void execute(const TestDefinitionPtr &test);

signals:
    void runningChanged(bool running);

    void started(const TestDefinitionPtr& test);
    void finished(const TestDefinitionPtr& test, const ResultPtr& result);

protected:
    class Private;
    Private* d;
};


#endif // TASKEXECUTOR_H
