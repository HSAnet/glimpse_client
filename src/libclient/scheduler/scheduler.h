#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../task/task.h"

class CLIENT_API Scheduler : public QObject
{
    Q_OBJECT

public:
    Scheduler();
    ~Scheduler();

    TestDefinitionList tests() const;

    void enqueue(const TestDefinitionPtr& testDefinition);
    void dequeue();

    void execute(const TestDefinitionPtr& testDefinition);

signals:
    void testAdded(const TestDefinitionPtr& test, int position);
    void testRemoved(const TestDefinitionPtr& test, int position);
    void testMoved(const TestDefinitionPtr& test, int from, int to);

protected:
    class Private;
    Private* d;
};

#endif // SCHEDULER_H
