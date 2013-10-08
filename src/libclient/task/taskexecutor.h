#ifndef TASKEXECUTOR_H
#define TASKEXECUTOR_H

#include "task.h"
#include "../report/report.h"
#include "../scheduler/scheduler.h" // TODO: REMOVE THIS

#include <QObject>

class TaskExecutor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
    TaskExecutor();
    ~TaskExecutor();

    bool isRunning() const;

    void execute(const TestDefinition& test);

signals:
    void runningChanged();

    void started(const TestDefinition& test);
    void finished(const TestDefinition& test, const Report& report);

protected:
    class Private;
    Private* d;
};


#endif // TASKEXECUTOR_H
