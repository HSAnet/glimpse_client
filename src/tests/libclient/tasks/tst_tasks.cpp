#include <QtTest>

#include "scheduler/scheduler.h"
#include "timing/immediatetiming.h"
#include "measurement/ping/ping.h"

class TestTasks : public QObject
{
    Q_OBJECT

    Scheduler scheduler;

private slots:
    void initTestCase()
    {
        QCOMPARE(scheduler.tasks(), TaskList());
    }

    void uniqueTasks()
    {
        TimingPtr timing(new ImmediateTiming());
        PingDefinition pingDef1("http://www.google.com", 1, 1000, 1000, 64, 1024, 1024, 64,
                               pingTypeFromString("System"));
        PingDefinition pingDef2("http://www.google.com", 2, 1000, 1000, 64, 1024, 1024, 64,
                               pingTypeFromString("System"));

        Task task1 = scheduler.nextImmidiateTask("ping", pingDef1.toVariant());
        ScheduleDefinition testDefinition1(ScheduleId(1), task1 , timing, Precondition());
        scheduler.enqueue(testDefinition1);
        QCOMPARE(scheduler.tasks().length(), 1);

        // this should create a new task
        Task task2 = scheduler.nextImmidiateTask("ping", pingDef2.toVariant());
        ScheduleDefinition testDefinition2(ScheduleId(2), task2, timing, Precondition());
        scheduler.enqueue(testDefinition2);
        QCOMPARE(scheduler.tasks().length(), 2);

        // this shouldn't create a new task
        Task task3 = scheduler.nextImmidiateTask("ping", pingDef1.toVariant());
        ScheduleDefinition testDefinition3(ScheduleId(3), task3, timing, Precondition());
        scheduler.enqueue(testDefinition3);
        QCOMPARE(scheduler.tasks().length(), 2);

        QCOMPARE(task1, task3);
    }
};

QTEST_MAIN(TestTasks)

#include "tst_tasks.moc"
