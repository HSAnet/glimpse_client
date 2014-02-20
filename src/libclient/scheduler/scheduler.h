#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../task/task.h"

class TaskExecutor;

class CLIENT_API Scheduler : public QObject
{
    Q_OBJECT

public:
    Scheduler();
    ~Scheduler();

    void setExecutor(TaskExecutor* executor);
    TaskExecutor* executor() const;

    TestDefinitionList tests() const;

    void enqueue(const TestDefinitionPtr& testDefinition);
    void dequeue();

    void execute(const TestDefinitionPtr& testDefinition);

    int executeOnDemandTest(const QUuid id);

signals:
    void testAdded(const TestDefinitionPtr& test, int position);
    void testRemoved(const TestDefinitionPtr& test, int position);
    void testMoved(const TestDefinitionPtr& test, int from, int to);

protected:
    class Private;
    Private* d;
};

#endif // SCHEDULER_H
