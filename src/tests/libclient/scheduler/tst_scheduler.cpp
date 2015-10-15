#include <QtTest>

#include <controller/ntpcontroller.h>

#include "scheduler/scheduler.h"
#include "task/taskexecutor.h"
#include "timing/periodictiming.h"
#include "measurement/ping/ping_definition.h"
#include "precondition.h"

class TestScheduler : public QObject
{
    Q_OBJECT

    Scheduler scheduler;
    TaskExecutor executor;
    NtpController ntp;

private slots:
    void initTestCase()
    {
        scheduler.setExecutor(&executor);
        ntp.init();
    }

    void initialGetters()
    {
        // lists should be empty
        QCOMPARE(scheduler.tests(), ScheduleDefinitionList());
        QCOMPARE(scheduler.tasks(), TaskList());


        QCOMPARE(scheduler.knownTestId(ScheduleId(0)), false);
        QCOMPARE(scheduler.taskByTaskId(TaskId(0)), Task());

        // is executor still the same?
        QCOMPARE(scheduler.executor(), &executor);
    }

    void enqueuing()
    {
        Precondition precondition;

        // enqueue first schedule
        TimingPtr timing(new PeriodicTiming(1000*60*10, QDateTime(QDate::currentDate().addDays(1), QTime(0,2,0))));
        ScheduleDefinition schedule(ScheduleId(11), TaskId(1), "ping", timing, PingDefinition::fromVariant(QVariant())->toVariant(), precondition);
        QCOMPARE(scheduler.enqueue(schedule), 0);

        // enqueue after first schedule
        timing = TimingPtr(new PeriodicTiming(1000*60*10, QDateTime(QDate::currentDate().addDays(1), QTime(0,3,0))));
        schedule = ScheduleDefinition(ScheduleId(12), TaskId(2), "ping", timing, PingDefinition::fromVariant(QVariant())->toVariant(), precondition);
        QCOMPARE(scheduler.enqueue(schedule), 1);

        // enqueue before first schedule
        timing = TimingPtr(new PeriodicTiming(1000*60*10, QDateTime(QDate::currentDate().addDays(1), QTime(0,1,0))));
        schedule = ScheduleDefinition(ScheduleId(13), TaskId(3), "ping", timing, PingDefinition::fromVariant(QVariant())->toVariant(), precondition);
        QCOMPARE(scheduler.enqueue(schedule), 0);

        // remove new first schedule
        scheduler.dequeue(ScheduleId(13));

        // check queue
        ScheduleDefinitionList queue = scheduler.queue();
        QCOMPARE(queue.length(), 2);
        QCOMPARE(queue.at(0).id(), ScheduleId(11));
        QCOMPARE(queue.at(1).id(), ScheduleId(12));

        // enqueue ended schedule
        timing = TimingPtr(new PeriodicTiming(1000*60*10, QDateTime(QDate::currentDate().addDays(1), QTime(0,1,0)), QDateTime(QDate::currentDate().addDays(-1), QTime(0,1,0))));
        schedule = ScheduleDefinition(ScheduleId(13), TaskId(3), "ping", timing, PingDefinition::fromVariant(QVariant())->toVariant(), precondition);
        QCOMPARE(scheduler.enqueue(schedule), -1);

        // empty queue
        scheduler.dequeue(ScheduleId(11));
        scheduler.dequeue(ScheduleId(12));
        QCOMPARE(scheduler.queue().length(), 0);

        // two schedules (hh:05 and hh:55)
        TimingPtr timingA = TimingPtr(new PeriodicTiming(1000*60*60, QDateTime(QDate::currentDate().addDays(-1), QTime(0,5,0))));
        TimingPtr timingB = TimingPtr(new PeriodicTiming(1000*60*60, QDateTime(QDate::currentDate().addDays(-1), QTime(0,55,0))));
        ScheduleDefinition scheduleA = ScheduleDefinition(ScheduleId(11), TaskId(1), "ping", timingA, PingDefinition::fromVariant(QVariant())->toVariant(), precondition);
        ScheduleDefinition scheduleB = ScheduleDefinition(ScheduleId(12), TaskId(2), "ping", timingB, PingDefinition::fromVariant(QVariant())->toVariant(), precondition);

        scheduler.enqueue(scheduleA);
        scheduler.enqueue(scheduleB);

        QDateTime now = ntp.currentDateTime();
        int minute = now.time().minute();

        queue = scheduler.queue();
        if (minute >= 5 && minute < 55)
        {
            QCOMPARE(queue.at(0).id(), ScheduleId(12));
            QCOMPARE(queue.at(1).id(), ScheduleId(11));
        }
        else
        {
            QCOMPARE(queue.at(0).id(), ScheduleId(11));
            QCOMPARE(queue.at(1).id(), ScheduleId(12));
        }
    }
};

QTEST_MAIN(TestScheduler)

#include "tst_scheduler.moc"
