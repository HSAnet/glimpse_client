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

    void setExecutor(TaskExecutor *executor);
    TaskExecutor *executor() const;

    TestDefinitionList tests() const;

    void enqueue(const TestDefinition &testDefinition);
    void dequeue();

    void execute(const TestDefinition &testDefinition);
    int executeOnDemandTest(const quint32 &id);

    bool knownTestId(const quint32 &id);

signals:
    void testAdded(const TestDefinition &test, int position);
    void testRemoved(const TestDefinition &test, int position);
    void testMoved(const TestDefinition &test, int from, int to);

protected:
    class Private;
    Private *d;
};

#endif // SCHEDULER_H
