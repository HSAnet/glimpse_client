#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../task/scheduledefinition.h"
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

    ScheduleDefinitionList tests() const;
    TaskList tasks() const;

    void addTask(const Task &task);
    void removeTask(const TaskId &taskId);

    int enqueue(const ScheduleDefinition &testDefinition);
    void dequeue(const ScheduleId &id);

    void execute(const ScheduleDefinition &testDefinition);
    int executeOnDemandTest(const ScheduleId &id);

    bool knownTestId(const ScheduleId &id);

    Task taskByTaskId(const TaskId &id) const;

    ScheduleDefinitionList queue() const;

    Task nextImmediateTask(const QString &method, const QVariant &measurementDefinition) const;

signals:
    void testAdded(const ScheduleDefinition &test, int position);
    void testRemoved(const ScheduleDefinition &test, int position);
    void testMoved(const ScheduleDefinition &test, int from, int to);
    void taskAdded(const Task &task);

protected:
    class Private;
    Private *d;
};

#endif // SCHEDULER_H
